#pragma once
#include <QWidget>
#include <memory>
#include <vector>

class MapaArequipa;
class Unidad;
class Incidente;

class MapaWidget : public QWidget {
    Q_OBJECT

public:
    explicit MapaWidget(const MapaArequipa& mapa, QWidget* parent = nullptr);

    void setUnidades(const std::vector<std::shared_ptr<Unidad>>& unidades);
    void setIncidentes(const std::vector<std::shared_ptr<Incidente>>& incidentes);
    void actualizar();

protected:
    void paintEvent(QPaintEvent* event) override;
    QSize sizeHint() const override;

private:
    void dibujarGrafo(QPainter& p);
    void dibujarUnidades(QPainter& p);
    void dibujarIncidentes(QPainter& p);
    QPointF nodoAPixel(int nodo) const;

    const MapaArequipa& m_mapa;
    std::vector<std::shared_ptr<Unidad>> m_unidades;
    std::vector<std::shared_ptr<Incidente>> m_incidentes;
};
