#pragma once
#include <QPixmap>
#include <QRectF>
#include <QWidget>
#include <memory>
#include <unordered_map>
#include <vector>
#include "prediction/ModeloPrediccion.h"

class MapaArequipa;
class Unidad;
class Incidente;
class QMouseEvent;
class QPaintEvent;
class QPainter;
class QPointF;
class QTimer;

class MapaWidget : public QWidget {
    Q_OBJECT

public:
    explicit MapaWidget(const MapaArequipa& mapa, QWidget* parent = nullptr);

    void setUnidades(const std::vector<std::shared_ptr<Unidad>>& unidades);
    void setIncidentes(const std::vector<std::shared_ptr<Incidente>>& incidentes);
    void setZonasRiesgo(const std::vector<PrediccionZona>& zonas);
    void cargarMapaFondo(const QString& ruta, double latNW, double lonNW, double latSE, double lonSE);
    void actualizar();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    QSize sizeHint() const override;

private:
    struct AnimacionUnidad {
        int nodoDesde;
        int nodoHasta;
        qreal progreso;
    };

    void dibujarFondo(QPainter& p);
    void dibujarGrafo(QPainter& p);
    void dibujarUnidades(QPainter& p);
    void dibujarIncidentes(QPainter& p);
    void avanzarAnimacion();
    QPointF nodoAPixel(int nodo) const;
    struct Transform { qreal escala, cx, cy; QRectF area; };
    Transform calcTransform() const;

    const MapaArequipa& m_mapa;
    std::vector<std::shared_ptr<Unidad>> m_unidades;
    std::vector<std::shared_ptr<Incidente>> m_incidentes;
    std::vector<PrediccionZona> m_zonasRiesgo;
    std::unordered_map<int, AnimacionUnidad> m_animaciones;
    QTimer* m_timerAnimacion = nullptr;
    int m_nodoSobrevolado = -1;

    QPixmap m_mapaFondo;
    QRectF  m_bboxFondo;   // en coordenadas del grafo (x,y)
};
