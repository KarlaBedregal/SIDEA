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

    QBoxLayout* m_contenido = nullptr;
    QLabel* m_lblTiempoPromedio = nullptr;
    QTableWidget* m_tablaUnidades = nullptr;
    QBarSet* m_atendidos = nullptr;
    QBarSet* m_pendientes = nullptr;
    QBarCategoryAxis* m_categorias = nullptr;
    QValueAxis* m_ejeIncidentes = nullptr;
};
