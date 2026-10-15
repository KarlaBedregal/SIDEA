#include "MapaWidget.h"
#include "graph/MapaArequipa.h"
#include "core/Unidad.h"
#include "core/Incidente.h"
#include <QPainter>
#include <QColor>

MapaWidget::MapaWidget(const MapaArequipa& mapa, QWidget* parent)
    : QWidget(parent), m_mapa(mapa) {
    setMinimumSize(800, 600);
}

void MapaWidget::setUnidades(const std::vector<std::shared_ptr<Unidad>>& unidades) {
    m_unidades = unidades;
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

QPointF MapaWidget::nodoAPixel(int nodo) const {
    auto pos = m_mapa.posicion(nodo);
    return {static_cast<qreal>(pos.x), static_cast<qreal>(pos.y)};
}

void MapaWidget::dibujarGrafo(QPainter& p) {
    // TODO Sprint 3: dibujar aristas y nodos del grafo
    p.setPen(QPen(Qt::gray, 1));
}

void MapaWidget::dibujarUnidades(QPainter& p) {
    // TODO Sprint 3: dibujar ícono de cada unidad en su nodo actual
}

void MapaWidget::dibujarIncidentes(QPainter& p) {
    // TODO Sprint 3: dibujar ícono de cada incidente activo
}
