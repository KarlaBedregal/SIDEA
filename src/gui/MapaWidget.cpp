#include "MapaWidget.h"
#include "graph/MapaArequipa.h"
#include "core/Unidad.h"
#include "core/Incidente.h"
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include <QFontMetricsF>
#include <QString>
#include <QTimer>
#include <QRectF>
#include <algorithm>
#include <cmath>
#include <limits>

MapaWidget::MapaWidget(const MapaArequipa& mapa, QWidget* parent)
    : QWidget(parent), m_mapa(mapa) {
    setMinimumSize(800, 600);
    setMouseTracking(true);

    m_timerAnimacion = new QTimer(this);
    m_timerAnimacion->setInterval(200);
    connect(m_timerAnimacion, &QTimer::timeout, this, &MapaWidget::avanzarAnimacion);
    m_timerAnimacion->start();
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
        if (unidadesActuales.find(it->first) == unidadesActuales.end()) {
            it = m_animaciones.erase(it);
        } else {
            ++it;
        }
    }
    actualizar();
}

void MapaWidget::setIncidentes(const std::vector<std::shared_ptr<Incidente>>& incidentes) {
    m_incidentes = incidentes;
}

void MapaWidget::actualizar() { update(); }

void MapaWidget::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
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
        const qreal distancia = std::sqrt(dx * dx + dy * dy);
        if (distancia < menorDistancia) {
            menorDistancia = distancia;
            nodoCercano = id;
        }
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
    auto pos = m_mapa.posicion(nodo);
    const auto nodos = m_mapa.grafo().nodos();
    if (nodos.empty()) return {};

    qreal minX = std::numeric_limits<qreal>::max();
    qreal maxX = std::numeric_limits<qreal>::lowest();
    qreal minY = std::numeric_limits<qreal>::max();
    qreal maxY = std::numeric_limits<qreal>::lowest();
    for (int id : nodos) {
        const auto posicion = m_mapa.posicion(id);
        minX = std::min(minX, static_cast<qreal>(posicion.x));
        maxX = std::max(maxX, static_cast<qreal>(posicion.x));
        minY = std::min(minY, static_cast<qreal>(posicion.y));
        maxY = std::max(maxY, static_cast<qreal>(posicion.y));
    }

    const QRectF area = QRectF(contentsRect()).adjusted(32.0, 32.0, -32.0, -32.0);
    if (area.width() <= 0.0 || area.height() <= 0.0) return area.center();

    const qreal anchoMapa = std::max(maxX - minX, 1.0);
    const qreal altoMapa = std::max(maxY - minY, 1.0);
    const qreal escala = std::min(area.width() / anchoMapa, area.height() / altoMapa);
    const qreal centroX = (minX + maxX) / 2.0;
    const qreal centroY = (minY + maxY) / 2.0;

    return {
        area.center().x() + (static_cast<qreal>(pos.x) - centroX) * escala,
        area.center().y() + (static_cast<qreal>(pos.y) - centroY) * escala
    };
}

void MapaWidget::dibujarGrafo(QPainter& p) {
    const auto& grafo = m_mapa.grafo();
    auto nodos = grafo.nodos();
    std::sort(nodos.begin(), nodos.end());

    // Las aristas bidireccionales aparecen desde ambos extremos; cada línea se dibuja una vez.
    p.setPen(QPen(QColor("#9AAABD"), 1.5));
    for (int origen : nodos) {
        for (const auto& arista : grafo.vecinos(origen)) {
            if (origen < arista.destino) {
                p.drawLine(nodoAPixel(origen), nodoAPixel(arista.destino));
            }
        }
    }

    p.setFont(QFont(QStringLiteral("Sans Serif"), 8));
    const QFontMetricsF metricas(p.font());
    const QRectF limites(contentsRect());
    constexpr qreal radio = 7.0;

    for (int id : nodos) {
        const QPointF centro = nodoAPixel(id);
        const auto posicion = m_mapa.posicion(id);

        const bool sobrevolado = id == m_nodoSobrevolado;
        p.setPen(QPen(sobrevolado ? QColor("#A65B00") : QColor("#17324D"), 1.5));
        p.setBrush(sobrevolado ? QColor("#FFD27A") : QColor("#E7F1FA"));
        p.drawEllipse(centro, sobrevolado ? radio + 2.0 : radio,
                      sobrevolado ? radio + 2.0 : radio);

        const QString nombre = QString::fromStdString(posicion.zona);
        const qreal anchoTexto = metricas.horizontalAdvance(nombre);
        const qreal altoTexto = metricas.height();
        const qreal separacion = radio + 5.0;
        qreal x = centro.x() + separacion;
        if (x + anchoTexto > limites.right() - 4.0) {
            x = centro.x() - separacion - anchoTexto;
        }
        const qreal y = std::clamp(
            centro.y() - altoTexto / 2.0,
            limites.top() + 2.0,
            std::max(limites.top() + 2.0, limites.bottom() - altoTexto - 2.0));

        p.setPen(QColor("#20364B"));
        p.setBrush(Qt::NoBrush);
        p.drawText(QRectF(x, y, anchoTexto + 2.0, altoTexto),
                   Qt::AlignLeft | Qt::AlignVCenter, nombre);
    }
}

void MapaWidget::dibujarUnidades(QPainter& p) {
    p.save();
    for (const auto& unidad : m_unidades) {
        if (!unidad) continue;

        QPointF centro = nodoAPixel(unidad->getNodoActual());
        const auto it = m_animaciones.find(unidad->getId());
        if (it != m_animaciones.end()) {
            const auto& animacion = it->second;
            const QPointF desde = nodoAPixel(animacion.nodoDesde);
            const QPointF hasta = nodoAPixel(animacion.nodoHasta);
            centro = desde + (hasta - desde) * animacion.progreso;
        }

        QColor color("#2E86AB");
        const std::string tipo = unidad->getTipoUnidad();
        if (tipo == "Ambulancia") color = QColor("#D64545");
        else if (tipo == "Bombero") color = QColor("#E67E22");
        else if (tipo == "Serenazgo") color = QColor("#2864A5");

        p.setPen(QPen(Qt::white, 2.0));
        p.setBrush(color);
        p.drawEllipse(centro, 8.0, 8.0);
    }
    p.restore();
}

void MapaWidget::avanzarAnimacion() {
    constexpr qreal incrementoPorTick = 0.4; // 200 ms por tick del mapa
    for (const auto& unidad : m_unidades) {
        if (!unidad) continue;

        const int id = unidad->getId();
        const int nodoActual = unidad->getNodoActual();
        auto it = m_animaciones.find(id);
        if (it == m_animaciones.end()) {
            m_animaciones.emplace(id, AnimacionUnidad{nodoActual, nodoActual, 1.0});
            continue;
        }

        auto& animacion = it->second;
        if (nodoActual != animacion.nodoHasta) {
            animacion = {animacion.nodoHasta, nodoActual, 0.0};
        } else if (animacion.progreso < 1.0) {
            animacion.progreso = std::min(1.0, animacion.progreso + incrementoPorTick);
        }
    }
    actualizar();
}

void MapaWidget::dibujarIncidentes(QPainter& p) {
    // TODO Sprint 3: dibujar ícono de cada incidente activo
}
