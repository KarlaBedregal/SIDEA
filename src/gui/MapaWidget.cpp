#include "MapaWidget.h"
#include "graph/MapaArequipa.h"
#include "core/Unidad.h"
#include "core/Incidente.h"
#include <QFile>
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include <QFontMetricsF>
#include <QString>
#include <QTextStream>
#include <QTimer>
#include <QRectF>
#include <algorithm>
#include <cmath>
#include <limits>

// Coordenadas geográficas del bounding box del grafo
static constexpr double GRAFO_LAT_MAX = -16.248;
static constexpr double GRAFO_LAT_MIN = -16.455;
static constexpr double GRAFO_LON_MIN = -71.657;
static constexpr double GRAFO_LON_MAX = -71.498;
static constexpr float  GRAFO_X_MIN   = 110.f;
static constexpr float  GRAFO_X_MAX   = 710.f;
static constexpr float  GRAFO_Y_MIN   = 80.f;
static constexpr float  GRAFO_Y_MAX   = 560.f;

void MapaWidget::setDuracionMovimiento(int ms) {
    if (m_timerAnimacion) m_timerAnimacion->setInterval(ms);
}

MapaWidget::MapaWidget(const MapaArequipa& mapa, QWidget* parent)
    : QWidget(parent), m_mapa(mapa) {
    setMinimumSize(800, 600);
    setMouseTracking(true);

    m_timerAnimacion = new QTimer(this);
    m_timerAnimacion->setInterval(200);
    connect(m_timerAnimacion, &QTimer::timeout, this, &MapaWidget::avanzarAnimacion);
    m_timerAnimacion->start();

    // Cargar mapa de fondo automáticamente si existe
    const QString rutaMapa = QStringLiteral("/home/karla/SIDEA/assets/mapa_arequipa.png");
    const QString rutaBbox = QStringLiteral("/home/karla/SIDEA/assets/mapa_bbox.txt");
    QFile bboxFile(rutaBbox);
    if (QFile::exists(rutaMapa) && bboxFile.open(QIODevice::ReadOnly)) {
        QTextStream ts(&bboxFile);
        double latNW, lonNW, latSE, lonSE;
        ts >> latNW >> lonNW >> latSE >> lonSE;
        if (ts.status() == QTextStream::Ok)
            cargarMapaFondo(rutaMapa, latNW, lonNW, latSE, lonSE);
    }
}

void MapaWidget::cargarMapaFondo(const QString& ruta, double latNW, double lonNW, double latSE, double lonSE) {
    m_mapaFondo.load(ruta);
    // Convertir esquinas geo de la imagen a coordenadas del grafo
    auto geoToGrafo = [](double lat, double lon) -> QPointF {
        double x = GRAFO_X_MIN + (lon - GRAFO_LON_MIN) / (GRAFO_LON_MAX - GRAFO_LON_MIN) * (GRAFO_X_MAX - GRAFO_X_MIN);
        double y = GRAFO_Y_MIN + (GRAFO_LAT_MAX - lat) / (GRAFO_LAT_MAX - GRAFO_LAT_MIN) * (GRAFO_Y_MAX - GRAFO_Y_MIN);
        return {x, y};
    };
    m_bboxFondo = QRectF(geoToGrafo(latNW, lonNW), geoToGrafo(latSE, lonSE));
    actualizar();
}

MapaWidget::Transform MapaWidget::calcTransform() const {
    const auto nodos = m_mapa.grafo().nodos();
    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();
    for (int id : nodos) {
        const auto pos = m_mapa.posicion(id);
        minX = std::min(minX, (qreal)pos.x);
        maxX = std::max(maxX, (qreal)pos.x);
        minY = std::min(minY, (qreal)pos.y);
        maxY = std::max(maxY, (qreal)pos.y);
    }
    const QRectF area = QRectF(contentsRect()).adjusted(32, 32, -32, -32);
    const qreal escala = (area.width() > 0 && area.height() > 0)
        ? std::min(area.width() / std::max(maxX - minX, 1.0),
                   area.height() / std::max(maxY - minY, 1.0))
        : 1.0;
    return { escala, (minX + maxX) / 2.0, (minY + maxY) / 2.0, area };
}

void MapaWidget::setUnidades(const std::vector<std::shared_ptr<Unidad>>& unidades) {
    m_unidades = unidades;
    std::unordered_map<int, bool> unidadesActuales;
    for (const auto& unidad : m_unidades) {
        if (!unidad) continue;
        const int id = unidad->getId();
        unidadesActuales.emplace(id, true);
        if (m_animaciones.find(id) == m_animaciones.end()) {
            const int nodo = unidad->getNodoActual();
            m_animaciones.emplace(id, AnimacionUnidad{nodo, nodo, 1.0});
        }
    }
    for (auto it = m_animaciones.begin(); it != m_animaciones.end();) {
        if (unidadesActuales.find(it->first) == unidadesActuales.end())
            it = m_animaciones.erase(it);
        else
            ++it;
    }
    actualizar();
}

void MapaWidget::setIncidentes(const std::vector<std::shared_ptr<Incidente>>& incidentes) {
    m_incidentes = incidentes;
    actualizar();
}

void MapaWidget::setZonasRiesgo(const std::vector<PrediccionZona>& zonas) {
    m_zonasRiesgo = zonas;
    actualizar();
}

void MapaWidget::actualizar() { update(); }

void MapaWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    dibujarFondo(p);
    dibujarGrafo(p);
    dibujarIncidentes(p);
    dibujarUnidades(p);
}

QSize MapaWidget::sizeHint() const { return {900, 650}; }

void MapaWidget::mouseMoveEvent(QMouseEvent* event) {
    int nodoCercano = -1;
    qreal menorDistancia = 14.0;
    const QPointF cursor = event->position();
    for (int id : m_mapa.grafo().nodos()) {
        const QPointF centro = nodoAPixel(id);
        const qreal dx = cursor.x() - centro.x();
        const qreal dy = cursor.y() - centro.y();
        const qreal dist = std::sqrt(dx * dx + dy * dy);
        if (dist < menorDistancia) { menorDistancia = dist; nodoCercano = id; }
    }
    if (nodoCercano != m_nodoSobrevolado) {
        m_nodoSobrevolado = nodoCercano;
        if (nodoCercano >= 0) {
            setToolTip(QString::fromStdString(m_mapa.nombreZona(nodoCercano)));
            setCursor(Qt::PointingHandCursor);
        } else {
            setToolTip(QString());
            unsetCursor();
        }
        actualizar();
    }
    QWidget::mouseMoveEvent(event);
}

QPointF MapaWidget::nodoAPixel(int nodo) const {
    const auto pos = m_mapa.posicion(nodo);
    const auto t = calcTransform();
    if (t.area.width() <= 0 || t.area.height() <= 0) return t.area.center();
    return {
        t.area.center().x() + ((qreal)pos.x - t.cx) * t.escala,
        t.area.center().y() + ((qreal)pos.y - t.cy) * t.escala
    };
}

void MapaWidget::dibujarFondo(QPainter& p) {
    if (m_mapaFondo.isNull() || !m_bboxFondo.isValid()) return;
    const auto t = calcTransform();
    if (t.area.width() <= 0 || t.area.height() <= 0) return;

    auto toWidget = [&](QPointF gp) -> QPointF {
        return { t.area.center().x() + (gp.x() - t.cx) * t.escala,
                 t.area.center().y() + (gp.y() - t.cy) * t.escala };
    };

    const QPointF wTL = toWidget(m_bboxFondo.topLeft());
    const QPointF wBR = toWidget(m_bboxFondo.bottomRight());
    const QRectF dest(wTL, wBR);

    p.setOpacity(0.70);
    p.drawPixmap(dest.toRect(), m_mapaFondo);
    p.setOpacity(1.0);
}

void MapaWidget::dibujarGrafo(QPainter& p) {
    const auto& grafo = m_mapa.grafo();
    auto nodos = grafo.nodos();
    std::sort(nodos.begin(), nodos.end());

    // Aristas — más visibles sobre el mapa
    p.setPen(QPen(QColor("#1A5276"), 2.0));
    for (int origen : nodos) {
        for (const auto& arista : grafo.vecinos(origen)) {
            if (origen < arista.destino)
                p.drawLine(nodoAPixel(origen), nodoAPixel(arista.destino));
        }
    }

    // Halos de zonas de riesgo
    for (std::size_t i = 0; i < m_zonasRiesgo.size(); ++i) {
        const QPointF centro = nodoAPixel(m_zonasRiesgo[i].nodo);
        const qreal radio = 16.0 + static_cast<qreal>(m_zonasRiesgo.size() - i) * 3.0;
        QColor color("#D84343");
        color.setAlpha(70);
        p.setPen(QPen(QColor("#B83232"), 2.0));
        p.setBrush(color);
        p.drawEllipse(centro, radio, radio);
    }

    p.setFont(QFont(QStringLiteral("Sans Serif"), 8));
    const QFontMetricsF metricas(p.font());
    const QRectF limites(contentsRect());
    constexpr qreal radio = 8.0;

    for (int id : nodos) {
        const QPointF centro = nodoAPixel(id);
        const auto posicion = m_mapa.posicion(id);
        const bool sobrevolado = id == m_nodoSobrevolado;
        const bool zonaRiesgo = std::any_of(m_zonasRiesgo.begin(), m_zonasRiesgo.end(),
            [id](const PrediccionZona& z) { return z.nodo == id; });

        p.setPen(QPen(sobrevolado ? QColor("#A65B00") :
                      (zonaRiesgo  ? QColor("#B83232") : QColor("#17324D")), 1.5));
        p.setBrush(sobrevolado ? QColor("#FFD27A") :
                   (zonaRiesgo  ? QColor("#FFE0E0") : QColor("#D6EAF8")));
        const qreal r = sobrevolado ? radio + 3.0 : radio;
        p.drawEllipse(centro, r, r);

        const QString nombre = QString::fromStdString(posicion.zona);
        const qreal aw = metricas.horizontalAdvance(nombre);
        const qreal ah = metricas.height();
        qreal x = centro.x() + r + 4.0;
        if (x + aw > limites.right() - 4.0) x = centro.x() - r - 4.0 - aw;
        const qreal y = std::clamp(centro.y() - ah / 2.0,
            limites.top() + 2.0,
            std::max(limites.top() + 2.0, limites.bottom() - ah - 2.0));

        // Fondo blanco semitransparente detrás del texto para legibilidad
        QColor bgTexto(255, 255, 255, 170);
        p.setBrush(bgTexto);
        p.setPen(Qt::NoPen);
        p.drawRoundedRect(QRectF(x - 1, y, aw + 2, ah), 2, 2);

        p.setPen(zonaRiesgo ? QColor("#8F2525") : QColor("#0A2744"));
        p.setBrush(Qt::NoBrush);
        p.drawText(QRectF(x, y, aw + 2.0, ah), Qt::AlignLeft | Qt::AlignVCenter, nombre);
    }

    if (!m_zonasRiesgo.empty()) {
        QFont fBold(QStringLiteral("Sans Serif"), 8);
        fBold.setBold(true);
        p.setFont(fBold);
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255, 255, 255, 180));
        p.drawRoundedRect(QRectF(limites.left() + 6, limites.top() + 4, 240, 20), 3, 3);
        p.setPen(QColor("#8F2525"));
        p.setBrush(Qt::NoBrush);
        p.drawText(QRectF(limites.left() + 8, limites.top() + 5, 238, 18),
                   Qt::AlignLeft | Qt::AlignVCenter,
                   QStringLiteral("● Zonas de mayor riesgo (historial)"));
    }
}

void MapaWidget::dibujarIncidentes(QPainter& p) {
    p.save();
    for (const auto& inc : m_incidentes) {
        if (!inc) continue;
        const QPointF pos = nodoAPixel(inc->getNodoUbicacion());

        QColor fondo("#FF8C00");
        fondo.setAlpha(210);
        p.setPen(QPen(QColor("#CC5500"), 2));
        p.setBrush(fondo);
        p.drawEllipse(pos, 11.0, 11.0);

        p.setPen(Qt::white);
        QFont f; f.setBold(true); f.setPixelSize(10);
        p.setFont(f);
        QString letra = "!";
        switch (inc->getTipo()) {
            case TipoIncidente::ACCIDENTE:       letra = "A"; break;
            case TipoIncidente::INCENDIO:        letra = "I"; break;
            case TipoIncidente::ROBO:            letra = "R"; break;
            case TipoIncidente::URGENCIA_MEDICA: letra = "M"; break;
        }
        p.drawText(QRectF(pos.x() - 5.5, pos.y() - 5.5, 11.0, 11.0), Qt::AlignCenter, letra);
    }
    p.restore();
}

void MapaWidget::dibujarUnidades(QPainter& p) {
    p.save();
    for (const auto& unidad : m_unidades) {
        if (!unidad) continue;

        QPointF centro = nodoAPixel(unidad->getNodoActual());
        const auto it = m_animaciones.find(unidad->getId());
        if (it != m_animaciones.end()) {
            const auto& anim = it->second;
            centro = nodoAPixel(anim.nodoDesde) + (nodoAPixel(anim.nodoHasta) - nodoAPixel(anim.nodoDesde)) * anim.progreso;
        }

        QColor color("#2E86AB");
        const std::string tipo = unidad->getTipoUnidad();
        if (tipo == "Ambulancia") color = QColor("#D64545");
        else if (tipo == "Bombero") color = QColor("#E67E22");
        else if (tipo == "Serenazgo") color = QColor("#2864A5");

        // Sombra
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 60));
        p.drawEllipse(centro + QPointF(1.5, 1.5), 8.0, 8.0);

        p.setPen(QPen(Qt::white, 2.0));
        p.setBrush(color);
        p.drawEllipse(centro, 8.0, 8.0);

        p.setPen(Qt::white);
        QFont f; f.setBold(true); f.setPixelSize(8);
        p.setFont(f);
        p.drawText(QRectF(centro.x() - 4.0, centro.y() - 4.0, 8.0, 8.0),
                   Qt::AlignCenter, QString::fromStdString(tipo).left(1));
    }
    p.restore();
}

void MapaWidget::avanzarAnimacion() {
    constexpr qreal inc = 0.4;
    for (const auto& unidad : m_unidades) {
        if (!unidad) continue;
        const int id = unidad->getId();
        const int nodoActual = unidad->getNodoActual();
        auto it = m_animaciones.find(id);
        if (it == m_animaciones.end()) {
            m_animaciones.emplace(id, AnimacionUnidad{nodoActual, nodoActual, 1.0});
            continue;
        }
        auto& anim = it->second;
        if (nodoActual != anim.nodoHasta)
            anim = {anim.nodoHasta, nodoActual, 0.0};
        else if (anim.progreso < 1.0)
            anim.progreso = std::min(1.0, anim.progreso + inc);
    }
    actualizar();
}
