#include "EstadisticasWidget.h"
#include "stats/Estadisticas.h"
#include <QVBoxLayout>
#include <QString>

EstadisticasWidget::EstadisticasWidget(QWidget* parent) : QWidget(parent) {
    configurarUI();
}

void EstadisticasWidget::configurarUI() {
    auto* layout = new QVBoxLayout(this);

    m_lblTiempoPromedio = new QLabel("Tiempo promedio: --");
    m_lblAtendidos      = new QLabel("Atendidos: 0");
    m_lblPendientes     = new QLabel("Pendientes: 0");

    m_tablaUnidades = new QTableWidget(0, 4);
    m_tablaUnidades->setHorizontalHeaderLabels({"Unidad", "Tipo", "Atendidos", "Uso %"});

    layout->addWidget(m_lblTiempoPromedio);
    layout->addWidget(m_lblAtendidos);
    layout->addWidget(m_lblPendientes);
    layout->addWidget(m_tablaUnidades);
}

void EstadisticasWidget::actualizar(const Estadisticas& stats) {
    m_lblTiempoPromedio->setText(
        QString("Tiempo promedio: %1 s").arg(stats.tiempoPromedioRespuesta(), 0, 'f', 1));
    m_lblAtendidos->setText(QString("Atendidos: %1").arg(stats.totalAtendidos()));
    m_lblPendientes->setText(QString("Pendientes: %1").arg(stats.totalPendientes()));
    // TODO Sprint 3: rellenar m_tablaUnidades con datosUnidades()
}

void EstadisticasWidget::limpiar() {
    m_lblTiempoPromedio->setText("Tiempo promedio: --");
    m_lblAtendidos->setText("Atendidos: 0");
    m_lblPendientes->setText("Pendientes: 0");
    m_tablaUnidades->setRowCount(0);
}
