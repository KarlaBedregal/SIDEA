#pragma once
#include <QWidget>
#include <QLabel>
#include <QTableWidget>

class Estadisticas;
class QBoxLayout;
class QBarSet;
class QBarCategoryAxis;
class QValueAxis;

class EstadisticasWidget : public QWidget {
    Q_OBJECT

public:
    explicit EstadisticasWidget(QWidget* parent = nullptr);

    void actualizar(const Estadisticas& stats);
    void limpiar();

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    void configurarUI();
    void exportarResumen();

    QBoxLayout* m_contenido;
    QLabel* m_lblTiempoPromedio;
    QTableWidget* m_tablaUnidades;
    QBarSet* m_atendidos;
    QBarSet* m_pendientes;
    QBarCategoryAxis* m_categorias;
    QValueAxis* m_ejeIncidentes;
};
