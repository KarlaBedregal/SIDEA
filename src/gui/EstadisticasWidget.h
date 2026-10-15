#pragma once
#include <QWidget>
#include <QLabel>
#include <QTableWidget>

class Estadisticas;

class EstadisticasWidget : public QWidget {
    Q_OBJECT

public:
    explicit EstadisticasWidget(QWidget* parent = nullptr);

    void actualizar(const Estadisticas& stats);
    void limpiar();

private:
    void configurarUI();

    QLabel* m_lblTiempoPromedio;
    QLabel* m_lblAtendidos;
    QLabel* m_lblPendientes;
    QTableWidget* m_tablaUnidades;
};
