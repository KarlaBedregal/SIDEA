#pragma once
#include <QPixmap>
#include <QPointF>
#include <QRectF>
#include <QWidget>
#include <memory>
#include <unordered_map>
#include <vector>
#include "prediction/ModeloPrediccion.h"

class MapaArequipa;
class Unidad;
class Incidente;
class QLabel;
class QMouseEvent;
class QPaintEvent;
class QPainter;
class QPushButton;
class QResizeEvent;
class QTimer;
class QWheelEvent;

class MapaWidget : public QWidget {
    Q_OBJECT

public:
    explicit MapaWidget(const MapaArequipa& mapa, QWidget* parent = nullptr);

    void setUnidades(const std::vector<std::shared_ptr<Unidad>>& unidades);
    void setIncidentes(const std::vector<std::shared_ptr<Incidente>>& incidentes);
    void setZonasRiesgo(const std::vector<PrediccionZona>& zonas);
    void cargarMapaFondo(const QString& ruta, double latNW, double lonNW, double latSE, double lonSE);
    void setDuracionMovimiento(int ms);
    void actualizar();

public slots:
    void acercar();
    void alejar();
    void restablecerVista();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    QSize sizeHint() const override;

private:
    struct AnimacionUnidad {
        int nodoDesde;
        int nodoHasta;
        qreal progreso;
    };

    void configurarControlesNavegacion();
    void posicionarControlesNavegacion();
    void aplicarZoom(qreal nuevoZoom, const QPointF& ancla);
    void limitarPan();
    void dibujarFondo(QPainter& p);
    void dibujarGrafo(QPainter& p);
    void dibujarRutasActivas(QPainter& p);
    void dibujarUnidades(QPainter& p);
    void dibujarIncidentes(QPainter& p);
    void dibujarLeyenda(QPainter& p);
    void dibujarTarjetaDireccion(QPainter& p);
    void avanzarAnimacion();
    int buscarNodoEnPosicion(const QPointF& pos) const;
    QRectF rectMapaBase() const;
    QRectF rectMapaDestino() const;
    QPointF nodoAPixel(int nodo) const;

    const MapaArequipa& m_mapa;
    std::vector<std::shared_ptr<Unidad>> m_unidades;
    std::vector<std::shared_ptr<Incidente>> m_incidentes;
    std::vector<PrediccionZona> m_zonasRiesgo;
    std::unordered_map<int, AnimacionUnidad> m_animaciones;
    QTimer* m_timerAnimacion = nullptr;
    int m_nodoSobrevolado = -1;
    int m_nodoSeleccionado = 0;
    int m_duracionMovimientoMs = 200;

    QPixmap m_mapaFondo;
    QRectF  m_bboxFondo;

    qreal m_zoom = 1.0;
    QPointF m_pan{0.0, 0.0};
    bool m_arrastrando = false;
    bool m_seArrastro = false;
    QPointF m_inicioArrastre;
    QPointF m_ultimoMouse;

    QWidget* m_barraZoom = nullptr;
    QPushButton* m_btnZoomIn = nullptr;
    QPushButton* m_btnZoomOut = nullptr;
    QPushButton* m_btnZoomReset = nullptr;
    QLabel* m_lblZoom = nullptr;
};
