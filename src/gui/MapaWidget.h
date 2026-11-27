#pragma once
#include <QWidget>
#include <QHash>
#include <QPixmap>
#include <QElapsedTimer>
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
class QTimer;

class MapaWidget : public QWidget {
    Q_OBJECT

public:
    explicit MapaWidget(const MapaArequipa& mapa, QWidget* parent = nullptr);

    void setUnidades(const std::vector<std::shared_ptr<Unidad>>& unidades);
    void setIncidentes(const std::vector<std::shared_ptr<Incidente>>& incidentes);
    void setZonasRiesgo(const std::vector<PrediccionZona>& zonas);
    void actualizar();
    void setDuracionMovimiento(int ms);

protected:
    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    QSize sizeHint() const override;

private:
    struct AnimacionUnidad {
        QPointF desde;
        int nodoHasta;
        qreal progreso;
    };

    void dibujarGrafo(QPainter& p);
    void dibujarUnidades(QPainter& p);
    void dibujarIncidentes(QPainter& p);
    void avanzarAnimacion();
    QPointF nodoAPixel(int nodo) const;

    const MapaArequipa& m_mapa;
    std::vector<std::shared_ptr<Unidad>> m_unidades;
    std::vector<std::shared_ptr<Incidente>> m_incidentes;
    std::vector<PrediccionZona> m_zonasRiesgo;
    std::unordered_map<int, AnimacionUnidad> m_animaciones;
    QTimer* m_timerAnimacion = nullptr;
    int m_nodoSobrevolado = -1;
    QHash<QString, QPixmap> m_iconos;
    QElapsedTimer m_relojAnimacion;
    int m_duracionMovimiento = 500;
};
