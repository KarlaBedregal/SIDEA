#include "MapaWidget.h"
#include "graph/MapaArequipa.h"
#include "core/Unidad.h"
#include "core/Incidente.h"
#include <QColor>
#include <QFontMetricsF>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QPushButton>
#include <QRectF>
#include <QResizeEvent>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QWheelEvent>
#include <algorithm>
#include <cmath>
#include <limits>

namespace {
constexpr qreal ZOOM_MIN = 0.8;
constexpr qreal ZOOM_MAX = 5.0;
constexpr qreal MAPA_IMG_W = 1280.0;
constexpr qreal MAPA_IMG_H = 1536.0;

QColor colorTipoUnidad(const std::string& tipo) {
    if (tipo == "Ambulancia") return QColor("#DC2626");
    if (tipo == "Bombero") return QColor("#EA580C");
    if (tipo == "Serenazgo") return QColor("#0284C7");
    return QColor("#0EA5E9");
}

QColor colorEstadoUnidad(const std::string& estado) {
    if (estado == "Libre") return QColor("#16A34A");
    if (estado == "EnRuta") return QColor("#D97706");
    if (estado == "Atendiendo") return QColor("#DB2777");
    if (estado == "Regresando") return QColor("#7C3AED");
    return QColor("#64748B");
}

QColor colorPrioridadIncidente(PrioridadIncidente prioridad) {
    switch (prioridad) {
        case PrioridadIncidente::CRITICA: return QColor("#DC2626");
        case PrioridadIncidente::ALTA: return QColor("#EA580C");
        case PrioridadIncidente::MEDIA: return QColor("#CA8A04");
        case PrioridadIncidente::BAJA: return QColor("#0284C7");
    }
    return QColor("#DC2626");
}

QString iconoUnidadPath(const std::string& tipo) {
    if (tipo == "Ambulancia") return QStringLiteral(":/icons/ambulancia.png");
    if (tipo == "Bombero") return QStringLiteral(":/icons/bombero.png");
    if (tipo == "Serenazgo") return QStringLiteral(":/icons/serenazgo.png");
    return {};
}

QString iconoIncidentePath(TipoIncidente tipo) {
    switch (tipo) {
        case TipoIncidente::INCENDIO: return QStringLiteral(":/icons/incendio.png");
        case TipoIncidente::ACCIDENTE: return QStringLiteral(":/icons/accidente.png");
        case TipoIncidente::ROBO: return QStringLiteral(":/icons/robo.png");
        case TipoIncidente::URGENCIA_MEDICA: return QStringLiteral(":/icons/urgencia_medica.png");
    }
    return {};
}

QString codigoIncidente(TipoIncidente tipo) {
    switch (tipo) {
        case TipoIncidente::INCENDIO: return "I";
        case TipoIncidente::ACCIDENTE: return "A";
        case TipoIncidente::ROBO: return "R";
        case TipoIncidente::URGENCIA_MEDICA: return "M";
    }
    return "!";
}

QString resumenCortoDireccion(const std::string& direccion) {
    const QString full = QString::fromStdString(direccion);
    const int slash = full.indexOf('/');
    QString parte = (slash > 0) ? full.left(slash).trimmed() : full;
    if (parte.size() > 26) parte = parte.left(24) + "...";
    return parte;
}
}

void MapaWidget::setDuracionMovimiento(int ms) {
    m_duracionMovimientoMs = qMax(16, ms);
}

MapaWidget::MapaWidget(const MapaArequipa& mapa, QWidget* parent)
    : QWidget(parent),
      m_mapa(mapa),
      m_mapaFondo(QStringLiteral(":/map/mapa_arequipa.png")) {
    setMinimumSize(780, 580);
    setMouseTracking(true);
    setCursor(Qt::OpenHandCursor);

    configurarControlesNavegacion();

    m_timerAnimacion = new QTimer(this);
    m_timerAnimacion->setInterval(16);
    connect(m_timerAnimacion, &QTimer::timeout, this, &MapaWidget::avanzarAnimacion);
    m_timerAnimacion->start();
}

void MapaWidget::cargarMapaFondo(const QString& ruta, double latNW, double lonNW, double latSE, double lonSE) {
    QPixmap cargado(ruta);
    if (!cargado.isNull()) {
        m_mapaFondo = std::move(cargado);
        m_bboxFondo = QRectF(QPointF(lonNW, latNW), QPointF(lonSE, latSE));
        actualizar();
    }
}

void MapaWidget::configurarControlesNavegacion() {
    m_barraZoom = new QWidget(this);
    m_barraZoom->setStyleSheet(
        "QWidget { background-color: rgba(255, 255, 255, 236); border: 1px solid #CBD5E1; border-radius: 6px; }"
        "QPushButton { background-color: #F1F5F9; color: #0F172A; font-weight: bold; border: 1px solid #CBD5E1; border-radius: 4px; padding: 4px 10px; min-width: 22px; }"
        "QPushButton:hover { background-color: #E0F2FE; border-color: #0284C7; color: #0369A1; }"
        "QLabel { border: none; background: transparent; color: #0F172A; font-weight: bold; padding: 0 4px; }");

    auto* layout = new QHBoxLayout(m_barraZoom);
    layout->setContentsMargins(6, 5, 6, 5);
    layout->setSpacing(5);

    m_btnZoomOut = new QPushButton("-", m_barraZoom);
    m_btnZoomOut->setToolTip("Alejar mapa (rueda abajo)");
    m_btnZoomOut->setCursor(Qt::PointingHandCursor);

    m_lblZoom = new QLabel("100%", m_barraZoom);
    m_lblZoom->setAlignment(Qt::AlignCenter);
    m_lblZoom->setMinimumWidth(42);

    m_btnZoomIn = new QPushButton("+", m_barraZoom);
    m_btnZoomIn->setToolTip("Acercar mapa (rueda arriba)");
    m_btnZoomIn->setCursor(Qt::PointingHandCursor);

    m_btnZoomReset = new QPushButton("Centrar", m_barraZoom);
    m_btnZoomReset->setToolTip("Restablecer zoom y centrar el mapa");
    m_btnZoomReset->setCursor(Qt::PointingHandCursor);

    layout->addWidget(m_btnZoomOut);
    layout->addWidget(m_lblZoom);
    layout->addWidget(m_btnZoomIn);
    layout->addWidget(m_btnZoomReset);

    m_barraZoom->adjustSize();
    posicionarControlesNavegacion();

    connect(m_btnZoomIn, &QPushButton::clicked, this, &MapaWidget::acercar);
    connect(m_btnZoomOut, &QPushButton::clicked, this, &MapaWidget::alejar);
    connect(m_btnZoomReset, &QPushButton::clicked, this, &MapaWidget::restablecerVista);
}

void MapaWidget::posicionarControlesNavegacion() {
    if (!m_barraZoom) return;
    m_barraZoom->adjustSize();
    m_barraZoom->move(std::max(12, width() - m_barraZoom->width() - 12), 10);
    m_barraZoom->raise();
}

void MapaWidget::acercar() {
    aplicarZoom(m_zoom * 1.25, QPointF(width() / 2.0, height() / 2.0));
}

void MapaWidget::alejar() {
    aplicarZoom(m_zoom / 1.25, QPointF(width() / 2.0, height() / 2.0));
}

void MapaWidget::restablecerVista() {
    m_zoom = 1.0;
    m_pan = QPointF(0.0, 0.0);
    if (m_lblZoom) m_lblZoom->setText("100%");
    actualizar();
}

void MapaWidget::aplicarZoom(qreal nuevoZoom, const QPointF& ancla) {
    nuevoZoom = std::clamp(nuevoZoom, ZOOM_MIN, ZOOM_MAX);
    if (std::abs(nuevoZoom - m_zoom) < 1e-4) return;

    const QRectF viejo = rectMapaDestino();
    if (viejo.width() <= 0.0 || viejo.height() <= 0.0) {
        m_zoom = nuevoZoom;
        actualizar();
        return;
    }

    const qreal u = (ancla.x() - viejo.left()) / viejo.width();
    const qreal v = (ancla.y() - viejo.top()) / viejo.height();

    m_zoom = nuevoZoom;
    const QRectF base = rectMapaBase();
    const qreal nuevoW = base.width() * m_zoom;
    const qreal nuevoH = base.height() * m_zoom;
    const qreal nuevoLeft = ancla.x() - u * nuevoW;
    const qreal nuevoTop = ancla.y() - v * nuevoH;

    m_pan = QPointF(nuevoLeft + nuevoW / 2.0 - base.center().x(),
                    nuevoTop + nuevoH / 2.0 - base.center().y());
    limitarPan();

    if (m_lblZoom) {
        m_lblZoom->setText(QString("%1%").arg(qRound(m_zoom * 100.0)));
    }
    actualizar();
}

void MapaWidget::limitarPan() {
    const QRectF base = rectMapaBase();
    const qreal mapaW = base.width() * m_zoom;
    const qreal mapaH = base.height() * m_zoom;
    const qreal maxPanX = std::max(width() * 0.45, mapaW * 0.5 + width() * 0.25);
    const qreal maxPanY = std::max(height() * 0.45, mapaH * 0.5 + height() * 0.25);
    m_pan.setX(std::clamp(m_pan.x(), -maxPanX, maxPanX));
    m_pan.setY(std::clamp(m_pan.y(), -maxPanY, maxPanY));
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
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    dibujarFondo(p);
    dibujarGrafo(p);
    dibujarRutasActivas(p);
    dibujarIncidentes(p);
    dibujarUnidades(p);
    dibujarLeyenda(p);
    dibujarTarjetaDireccion(p);
}

void MapaWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    limitarPan();
    posicionarControlesNavegacion();
}

QSize MapaWidget::sizeHint() const { return {940, 700}; }

int MapaWidget::buscarNodoEnPosicion(const QPointF& cursor) const {
    int nodoCercano = -1;
    qreal menorDistancia = 20.0;
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
    return nodoCercano;
}

void MapaWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_arrastrando) {
        const QPointF pos = event->position();
        const QPointF total = pos - m_inicioArrastre;
        if (std::hypot(total.x(), total.y()) > 4.0) {
            m_seArrastro = true;
            setCursor(Qt::ClosedHandCursor);
        }
        m_pan += (pos - m_ultimoMouse);
        m_ultimoMouse = pos;
        limitarPan();
        actualizar();
        QWidget::mouseMoveEvent(event);
        return;
    }

    const int nodoCercano = buscarNodoEnPosicion(event->position());

    if (nodoCercano != m_nodoSobrevolado) {
        m_nodoSobrevolado = nodoCercano;
        if (nodoCercano >= 0) {
            const auto pos = m_mapa.posicion(nodoCercano);
            QString info = QString("Zona %1: %2\nDirección: %3\nGPS: %4, %5")
                               .arg(nodoCercano)
                               .arg(QString::fromStdString(pos.zona),
                                    QString::fromStdString(pos.direccion))
                               .arg(pos.latitud, 0, 'f', 4)
                               .arg(pos.longitud, 0, 'f', 4);
            for (const auto& u : m_unidades) {
                if (u && u->getNodoActual() == nodoCercano) {
                    info += QString("\n• %1 (%2 - %3)")
                                .arg(QString::fromStdString(u->getNombre()),
                                     QString::fromStdString(u->getTipoUnidad()),
                                     QString::fromStdString(u->getEstadoNombre()));
                }
            }
            for (const auto& inc : m_incidentes) {
                if (inc && !inc->estaAtendido() && inc->getNodoUbicacion() == nodoCercano) {
                    info += QString("\n! Incidente #%1: %2 (%3)")
                                .arg(inc->getId())
                                .arg(QString::fromStdString(inc->tipoToString()),
                                     QString::fromStdString(inc->prioridadToString()));
                }
            }
            setToolTip(info);
            setCursor(Qt::PointingHandCursor);
        } else {
            setToolTip(QString());
            setCursor(Qt::OpenHandCursor);
        }
        actualizar();
    }

    QWidget::mouseMoveEvent(event);
}

void MapaWidget::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton) {
        m_arrastrando = true;
        m_seArrastro = false;
        m_inicioArrastre = event->position();
        m_ultimoMouse = event->position();
    }
    QWidget::mousePressEvent(event);
}

void MapaWidget::mouseReleaseEvent(QMouseEvent* event) {
    if (m_arrastrando && (event->button() == Qt::LeftButton || event->button() == Qt::MiddleButton)) {
        if (!m_seArrastro && event->button() == Qt::LeftButton) {
            const int nodo = buscarNodoEnPosicion(event->position());
            if (nodo >= 0) {
                m_nodoSeleccionado = nodo;
                actualizar();
            }
        }
        m_arrastrando = false;
        m_seArrastro = false;
        setCursor(m_nodoSobrevolado >= 0 ? Qt::PointingHandCursor : Qt::OpenHandCursor);
    }
    QWidget::mouseReleaseEvent(event);
}

void MapaWidget::wheelEvent(QWheelEvent* event) {
    const int delta = event->angleDelta().y();
    if (delta != 0) {
        const qreal factor = (delta > 0) ? 1.18 : (1.0 / 1.18);
        aplicarZoom(m_zoom * factor, event->position());
        event->accept();
        return;
    }
    QWidget::wheelEvent(event);
}

// Encuadra la caja geográfica que contiene los 20 nodos + la imagen mapa_arequipa.png (0..1280 x 0..1536)
QRectF MapaWidget::rectMapaBase() const {
    qreal minX = 0.0;
    qreal maxX = MAPA_IMG_W;
    qreal minY = 0.0;
    qreal maxY = MAPA_IMG_H;
    for (int id : m_mapa.grafo().nodos()) {
        const auto pos = m_mapa.posicion(id);
        minX = std::min(minX, static_cast<qreal>(pos.x));
        maxX = std::max(maxX, static_cast<qreal>(pos.x));
        minY = std::min(minY, static_cast<qreal>(pos.y));
        maxY = std::max(maxY, static_cast<qreal>(pos.y));
    }
    const QRectF mundo = QRectF(minX - 60.0, minY - 60.0,
                                (maxX - minX) + 180.0, (maxY - minY) + 140.0);
    const QRectF lienzo = QRectF(contentsRect());
    if (lienzo.width() <= 0.0 || lienzo.height() <= 0.0) return lienzo;

    const qreal escala = std::min(lienzo.width() / mundo.width(), lienzo.height() / mundo.height());
    const qreal w = mundo.width() * escala;
    const qreal h = mundo.height() * escala;
    return QRectF(lienzo.center().x() - w / 2.0, lienzo.center().y() - h / 2.0, w, h);
}

QRectF MapaWidget::rectMapaDestino() const {
    const QRectF base = rectMapaBase();
    const qreal w = base.width() * m_zoom;
    const qreal h = base.height() * m_zoom;
    const QPointF c = base.center();
    return QRectF(c.x() - w / 2.0 + m_pan.x(), c.y() - h / 2.0 + m_pan.y(), w, h);
}

QPointF MapaWidget::nodoAPixel(int nodo) const {
    qreal minX = 0.0, maxX = MAPA_IMG_W, minY = 0.0, maxY = MAPA_IMG_H;
    for (int id : m_mapa.grafo().nodos()) {
        const auto p = m_mapa.posicion(id);
        minX = std::min(minX, static_cast<qreal>(p.x));
        maxX = std::max(maxX, static_cast<qreal>(p.x));
        minY = std::min(minY, static_cast<qreal>(p.y));
        maxY = std::max(maxY, static_cast<qreal>(p.y));
    }
    const QRectF mundo(minX - 60.0, minY - 60.0,
                       (maxX - minX) + 180.0, (maxY - minY) + 140.0);
    const QRectF destino = rectMapaDestino();
    const auto pos = m_mapa.posicion(nodo);
    return {
        destino.left() + ((static_cast<qreal>(pos.x) - mundo.left()) / mundo.width()) * destino.width(),
        destino.top() + ((static_cast<qreal>(pos.y) - mundo.top()) / mundo.height()) * destino.height()
    };
}

void MapaWidget::dibujarFondo(QPainter& p) {
    // Mismo tono cálido claro del borde de mapa_arequipa.png para transición continua
    p.fillRect(rect(), QColor("#F2EFE9"));
    if (m_mapaFondo.isNull()) return;

    qreal minX = 0.0, maxX = MAPA_IMG_W, minY = 0.0, maxY = MAPA_IMG_H;
    for (int id : m_mapa.grafo().nodos()) {
        const auto pos = m_mapa.posicion(id);
        minX = std::min(minX, static_cast<qreal>(pos.x));
        maxX = std::max(maxX, static_cast<qreal>(pos.x));
        minY = std::min(minY, static_cast<qreal>(pos.y));
        maxY = std::max(maxY, static_cast<qreal>(pos.y));
    }
    const QRectF mundo(minX - 60.0, minY - 60.0,
                       (maxX - minX) + 180.0, (maxY - minY) + 140.0);
    const QRectF destino = rectMapaDestino();

    const qreal imgW = static_cast<qreal>(m_mapaFondo.width());
    const qreal imgH = static_cast<qreal>(m_mapaFondo.height());
    const QRectF imgEnWidget(
        destino.left() + ((0.0 - mundo.left()) / mundo.width()) * destino.width(),
        destino.top() + ((0.0 - mundo.top()) / mundo.height()) * destino.height(),
        (imgW / mundo.width()) * destino.width(),
        (imgH / mundo.height()) * destino.height());

    p.drawPixmap(imgEnWidget, m_mapaFondo, QRectF(m_mapaFondo.rect()));
    // Suave velo claro para que las vías del grafo, unidades y etiquetas resalten con claridad
    p.fillRect(imgEnWidget, QColor(255, 255, 255, 42));
}

void MapaWidget::dibujarGrafo(QPainter& p) {
    const auto& grafo = m_mapa.grafo();
    auto nodos = grafo.nodos();
    std::sort(nodos.begin(), nodos.end());

    const int nodoActivo = (m_nodoSobrevolado >= 0) ? m_nodoSobrevolado : m_nodoSeleccionado;

    for (int origen : nodos) {
        for (const auto& arista : grafo.vecinos(origen)) {
            if (origen < arista.destino) {
                const bool esConectada = (origen == nodoActivo || arista.destino == nodoActivo);
                const QPointF p1 = nodoAPixel(origen);
                const QPointF p2 = nodoAPixel(arista.destino);
                p.setPen(QPen(esConectada ? QColor(2, 132, 199, 225) : QColor(30, 58, 138, 120),
                              esConectada ? 2.5 : 1.5));
                p.drawLine(p1, p2);

                if ((esConectada || m_zoom >= 1.8) && !arista.datos.nombre.empty()) {
                    const QPointF medio((p1.x() + p2.x()) / 2.0, (p1.y() + p2.y()) / 2.0);
                    QFont fVia(QStringLiteral("Segoe UI"), 7, QFont::DemiBold);
                    p.setFont(fVia);
                    const QFontMetricsF fmVia(fVia);
                    const QString txtVia = QString("%1 (%2 km)")
                                               .arg(QString::fromStdString(arista.datos.nombre))
                                               .arg(arista.peso, 0, 'f', 1);
                    const QRectF rVia(medio.x() - fmVia.horizontalAdvance(txtVia) / 2.0 - 4.0,
                                      medio.y() - fmVia.height() / 2.0 - 1.0,
                                      fmVia.horizontalAdvance(txtVia) + 8.0,
                                      fmVia.height() + 2.0);
                    p.setPen(QPen(QColor("#BAE6FD"), 0.8));
                    p.setBrush(QColor(255, 255, 255, 238));
                    p.drawRoundedRect(rVia, 3.0, 3.0);
                    p.setPen(esConectada ? QColor("#0369A1") : QColor("#334155"));
                    p.drawText(rVia, Qt::AlignCenter, txtVia);
                }
            }
        }
    }

    // Halos de zonas de riesgo (predicción histórica)
    for (std::size_t i = 0; i < m_zonasRiesgo.size(); ++i) {
        const QPointF centro = nodoAPixel(m_zonasRiesgo[i].nodo);
        const qreal radio = 18.0 + static_cast<qreal>(m_zonasRiesgo.size() - i) * 3.5;
        QColor color("#DC2626");
        color.setAlpha(55);
        p.setPen(QPen(QColor("#B91C1C"), 2.0));
        p.setBrush(color);
        p.drawEllipse(centro, radio, radio);
    }

    QFont fuenteZona(QStringLiteral("Segoe UI"), 8, QFont::Bold);
    QFont fuenteDir(QStringLiteral("Segoe UI"), 7, QFont::Normal);
    const QFontMetricsF fmZona(fuenteZona);
    const QFontMetricsF fmDir(fuenteDir);
    const QRectF limites(contentsRect());
    constexpr qreal radio = 6.5;
    const bool mostrarTodasDirecciones = (m_zoom >= 1.55);

    for (int id : nodos) {
        const QPointF centro = nodoAPixel(id);
        if (!limites.adjusted(-80, -80, 80, 80).contains(centro)) continue;

        const auto pos = m_mapa.posicion(id);
        const bool sobrevolado = (id == m_nodoSobrevolado);
        const bool seleccionado = (id == m_nodoSeleccionado);
        const bool destacado = sobrevolado || seleccionado;
        const bool zonaRiesgo = std::any_of(m_zonasRiesgo.begin(), m_zonasRiesgo.end(),
            [id](const PrediccionZona& z) { return z.nodo == id; });
        const bool conDireccion = destacado || mostrarTodasDirecciones;

        if (destacado) {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(245, 158, 11, 70));
            p.drawEllipse(centro, radio + 9.0, radio + 9.0);
        }

        p.setPen(QPen(destacado ? QColor("#B45309")
                                : (zonaRiesgo ? QColor("#B91C1C") : QColor("#1E3A8A")),
                      destacado ? 2.2 : 1.6));
        p.setBrush(destacado ? QColor("#FEF08A")
                             : (zonaRiesgo ? QColor("#FEE2E2") : QColor("#FFFFFF")));
        const qreal r = destacado ? radio + 2.0 : radio;
        p.drawEllipse(centro, r, r);

        const QString nombre = QString::fromStdString(pos.zona);
        const QString dirCorta = resumenCortoDireccion(pos.direccion);
        const qreal ancho = std::max(fmZona.horizontalAdvance(nombre),
                                     conDireccion ? fmDir.horizontalAdvance(dirCorta) : 0.0);
        const qreal alto = conDireccion ? (fmZona.height() + fmDir.height() + 4.0)
                                        : (fmZona.height() + 3.0);
        const qreal padX = 5.0;
        const qreal separacion = radio + 6.0;

        qreal x = centro.x() + separacion;
        if (x + ancho + padX * 2 > limites.right() - 8.0) {
            x = centro.x() - separacion - ancho - padX * 2;
        }
        const qreal y = std::clamp(
            centro.y() - alto / 2.0,
            limites.top() + 4.0,
            std::max(limites.top() + 4.0, limites.bottom() - alto - 4.0));

        const QRectF caja(x, y, ancho + padX * 2, alto);
        p.setPen(QPen(destacado ? QColor("#D97706")
                                : (zonaRiesgo ? QColor("#DC2626") : QColor("#94A3B8")),
                      destacado ? 1.5 : 1.0));
        p.setBrush(destacado ? QColor(255, 251, 235, 245) : QColor(255, 255, 255, 232));
        p.drawRoundedRect(caja, 4.0, 4.0);

        p.setFont(fuenteZona);
        p.setPen(destacado ? QColor("#92400E")
                           : (zonaRiesgo ? QColor("#991B1B") : QColor("#0F172A")));
        if (conDireccion) {
            const QRectF rTop(caja.left() + padX, caja.top() + 1.0, ancho, fmZona.height());
            const QRectF rBot(caja.left() + padX, rTop.bottom(), ancho, fmDir.height());
            p.drawText(rTop, Qt::AlignLeft | Qt::AlignVCenter, nombre);
            p.setFont(fuenteDir);
            p.setPen(destacado ? QColor("#0369A1") : QColor("#475569"));
            p.drawText(rBot, Qt::AlignLeft | Qt::AlignVCenter, dirCorta);
        } else {
            p.drawText(caja, Qt::AlignCenter, nombre);
        }
    }
}

void MapaWidget::dibujarRutasActivas(QPainter& p) {
    p.save();
    for (const auto& unidad : m_unidades) {
        if (!unidad) continue;
        const auto ruta = unidad->getRuta();
        if (ruta.size() < 2) continue;

        p.setPen(QPen(QColor(255, 255, 255, 210), 6.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        for (std::size_t i = 0; i + 1 < ruta.size(); ++i) {
            p.drawLine(nodoAPixel(ruta[i]), nodoAPixel(ruta[i + 1]));
        }

        QColor core = colorTipoUnidad(unidad->getTipoUnidad());
        p.setPen(QPen(core, 3.0, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin));
        for (std::size_t i = 0; i + 1 < ruta.size(); ++i) {
            p.drawLine(nodoAPixel(ruta[i]), nodoAPixel(ruta[i + 1]));
        }
    }
    p.restore();
}

void MapaWidget::dibujarIncidentes(QPainter& p) {
    p.save();
    std::unordered_map<int, int> conteoPorNodo;
    QFont fuenteTag(QStringLiteral("Segoe UI"), 8, QFont::Bold);
    p.setFont(fuenteTag);

    for (const auto& inc : m_incidentes) {
        if (!inc || inc->estaAtendido()) continue;
        const int nodo = inc->getNodoUbicacion();
        const int idx = conteoPorNodo[nodo]++;
        const QPointF base = nodoAPixel(nodo);
        const QPointF centro(base.x() - 15.0 - idx * 15.0, base.y() - 15.0);

        const QColor c = colorPrioridadIncidente(inc->getPrioridad());
        QColor halo = c;
        halo.setAlpha(75);
        p.setPen(Qt::NoPen);
        p.setBrush(halo);
        p.drawEllipse(centro, 15.0, 15.0);

        QPolygonF rombo;
        rombo << QPointF(centro.x(), centro.y() - 11.0)
              << QPointF(centro.x() + 11.0, centro.y())
              << QPointF(centro.x(), centro.y() + 11.0)
              << QPointF(centro.x() - 11.0, centro.y());
        p.setPen(QPen(Qt::white, 1.8));
        p.setBrush(c);
        p.drawPolygon(rombo);

        const QPixmap icono(iconoIncidentePath(inc->getTipo()));
        if (!icono.isNull()) {
            p.drawPixmap(QRectF(centro.x() - 7.0, centro.y() - 7.0, 14.0, 14.0).toRect(), icono);
        } else {
            p.setPen(Qt::white);
            p.drawText(QRectF(centro.x() - 10.0, centro.y() - 10.0, 20.0, 20.0),
                       Qt::AlignCenter, codigoIncidente(inc->getTipo()));
        }
    }
    p.restore();
}

void MapaWidget::dibujarUnidades(QPainter& p) {
    p.save();
    QFont fuenteNombre(QStringLiteral("Segoe UI"), 8, QFont::Bold);
    p.setFont(fuenteNombre);
    const QFontMetricsF metricas(fuenteNombre);

    std::unordered_map<int, int> conteoEstatico;
    for (const auto& unidad : m_unidades) {
        if (!unidad) continue;

        QPointF centro = nodoAPixel(unidad->getNodoActual());
        bool enMovimiento = false;
        const auto it = m_animaciones.find(unidad->getId());
        if (it != m_animaciones.end()) {
            const auto& animacion = it->second;
            const QPointF desde = nodoAPixel(animacion.nodoDesde);
            const QPointF hasta = nodoAPixel(animacion.nodoHasta);
            centro = desde + (hasta - desde) * animacion.progreso;
            enMovimiento = (animacion.nodoDesde != animacion.nodoHasta && animacion.progreso < 1.0);
        }

        if (!enMovimiento) {
            const int idx = conteoEstatico[unidad->getNodoActual()]++;
            if (idx > 0) {
                const qreal angulo = idx * 1.15;
                centro += QPointF(std::cos(angulo) * 18.0, std::sin(angulo) * 18.0);
            }
        }

        const std::string tipo = unidad->getTipoUnidad();
        const std::string estado = unidad->getEstadoNombre();
        const QColor cTipo = colorTipoUnidad(tipo);
        const QColor cEstado = colorEstadoUnidad(estado);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(15, 23, 42, 65));
        p.drawEllipse(centro + QPointF(1.5, 2.0), 13.0, 13.0);

        p.setPen(QPen(cEstado, 2.8));
        p.setBrush(cTipo);
        p.drawEllipse(centro, 11.5, 11.5);

        const QPixmap icono(iconoUnidadPath(tipo));
        if (!icono.isNull()) {
            p.drawPixmap(QRectF(centro.x() - 8.0, centro.y() - 8.0, 16.0, 16.0).toRect(), icono);
        } else {
            const QString letra = tipo.empty() ? "?" : QString(QChar(tipo[0]));
            p.setPen(Qt::white);
            p.drawText(QRectF(centro.x() - 11.0, centro.y() - 11.0, 22.0, 22.0),
                       Qt::AlignCenter, letra);
        }

        const QString etiqueta = QString::fromStdString(unidad->getNombre());
        const qreal ancho = metricas.horizontalAdvance(etiqueta) + 8.0;
        const qreal alto = metricas.height() + 2.0;
        const QRectF caja(centro.x() - ancho / 2.0, centro.y() + 13.0, ancho, alto);
        p.setPen(QPen(cEstado, 1.2));
        p.setBrush(QColor(255, 255, 255, 240));
        p.drawRoundedRect(caja, 3.0, 3.0);
        p.setPen(QColor("#0F172A"));
        p.drawText(caja, Qt::AlignCenter, etiqueta);
    }
    p.restore();
}

void MapaWidget::dibujarLeyenda(QPainter& p) {
    p.save();
    QFont fuente(QStringLiteral("Segoe UI"), 8, QFont::Medium);
    p.setFont(fuente);

    int activos = 0;
    for (const auto& inc : m_incidentes) {
        if (inc && !inc->estaAtendido()) ++activos;
    }

    const QString resumen = QString("Mapa Arequipa (%1 zonas)  |  Unidades: %2  |  Incidentes activos: %3")
                                .arg(m_mapa.totalNodos())
                                .arg(m_unidades.size())
                                .arg(activos);
    const QFontMetricsF fm(fuente);
    const QRectF cajaTop(12.0, 10.0, fm.horizontalAdvance(resumen) + 18.0, fm.height() + 8.0);
    p.setPen(QPen(QColor("#CBD5E1"), 1.0));
    p.setBrush(QColor(255, 255, 255, 236));
    p.drawRoundedRect(cajaTop, 6.0, 6.0);
    p.setPen(QColor("#0F172A"));
    p.drawText(cajaTop, Qt::AlignCenter, resumen);

    const QRectF cajaLeyenda(12.0, height() - 92.0, 290.0, 80.0);
    p.setPen(QPen(QColor("#CBD5E1"), 1.0));
    p.setBrush(QColor(255, 255, 255, 236));
    p.drawRoundedRect(cajaLeyenda, 6.0, 6.0);

    struct Item { QColor color; QString texto; bool rombo; };
    const std::vector<Item> items = {
        {QColor("#DC2626"), "Ambulancia", false},
        {QColor("#EA580C"), "Bombero", false},
        {QColor("#0284C7"), "Serenazgo", false},
        {QColor("#DC2626"), "Incidente activo", true}
    };

    for (int i = 0; i < static_cast<int>(items.size()); ++i) {
        const qreal col = (i % 2) * 140.0;
        const qreal fila = (i / 2) * 26.0;
        const QPointF pt(cajaLeyenda.left() + 18.0 + col, cajaLeyenda.top() + 24.0 + fila);
        p.setPen(QPen(Qt::white, 1.0));
        p.setBrush(items[static_cast<std::size_t>(i)].color);
        if (items[static_cast<std::size_t>(i)].rombo) {
            QPolygonF r;
            r << QPointF(pt.x(), pt.y() - 6.0) << QPointF(pt.x() + 6.0, pt.y())
              << QPointF(pt.x(), pt.y() + 6.0) << QPointF(pt.x() - 6.0, pt.y());
            p.drawPolygon(r);
        } else {
            p.drawEllipse(pt, 6.0, 6.0);
        }
        p.setPen(QColor("#0F172A"));
        p.drawText(QRectF(pt.x() + 12.0, pt.y() - 9.0, 120.0, 18.0),
                   Qt::AlignLeft | Qt::AlignVCenter, items[static_cast<std::size_t>(i)].texto);
    }

    p.setPen(QColor("#475569"));
    p.drawText(QRectF(cajaLeyenda.left() + 12.0, cajaLeyenda.bottom() - 22.0, 268.0, 18.0),
               Qt::AlignLeft | Qt::AlignVCenter,
               "Arrastra para mover · Rueda/botones para zoom");
    p.restore();
}

void MapaWidget::dibujarTarjetaDireccion(QPainter& p) {
    const int nodo = (m_nodoSobrevolado >= 0) ? m_nodoSobrevolado : m_nodoSeleccionado;
    if (!m_mapa.grafo().existeNodo(nodo)) return;

    const auto pos = m_mapa.posicion(nodo);
    QStringList vias;
    for (const auto& arista : m_mapa.grafo().vecinos(nodo)) {
        vias << QString("%1 (%2 km)")
                    .arg(QString::fromStdString(arista.datos.nombre))
                    .arg(arista.peso, 0, 'f', 1);
    }

    const qreal ancho = 365.0;
    const qreal alto = 94.0;
    const QRectF tarjeta(width() - ancho - 12.0, height() - alto - 12.0, ancho, alto);
    if (tarjeta.left() < 312.0) return;

    p.save();
    p.setPen(QPen(QColor("#0284C7"), 1.3));
    p.setBrush(QColor(255, 255, 255, 242));
    p.drawRoundedRect(tarjeta, 7.0, 7.0);

    QFont fTitulo(QStringLiteral("Segoe UI"), 9, QFont::Bold);
    QFont fTexto(QStringLiteral("Segoe UI"), 8, QFont::Medium);
    QFont fSub(QStringLiteral("Segoe UI"), 7, QFont::Normal);

    p.setFont(fTitulo);
    p.setPen(QColor("#0F172A"));
    p.drawText(QRectF(tarjeta.left() + 10.0, tarjeta.top() + 6.0, ancho - 20.0, 18.0),
               Qt::AlignLeft | Qt::AlignVCenter,
               QString("Punto #%1: %2").arg(nodo).arg(QString::fromStdString(pos.zona)));

    p.setFont(fTexto);
    p.setPen(QColor("#1E293B"));
    p.drawText(QRectF(tarjeta.left() + 10.0, tarjeta.top() + 26.0, ancho - 20.0, 18.0),
               Qt::AlignLeft | Qt::AlignVCenter,
               QString::fromStdString(pos.direccion));

    p.setFont(fSub);
    p.setPen(QColor("#0369A1"));
    p.drawText(QRectF(tarjeta.left() + 10.0, tarjeta.top() + 46.0, ancho - 20.0, 16.0),
               Qt::AlignLeft | Qt::AlignVCenter,
               QString("GPS: %1, %2  |  Clic en punto para fijar")
                   .arg(pos.latitud, 0, 'f', 4)
                   .arg(pos.longitud, 0, 'f', 4));

    p.setPen(QColor("#475569"));
    p.drawText(QRectF(tarjeta.left() + 10.0, tarjeta.top() + 64.0, ancho - 20.0, 22.0),
               Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine,
               "Vías: " + vias.join(" · "));
    p.restore();
}

void MapaWidget::avanzarAnimacion() {
    const qreal inc = 16.0 / m_duracionMovimientoMs;
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
            animacion.progreso = std::min(1.0, animacion.progreso + inc);
        }
    }
    actualizar();
}
