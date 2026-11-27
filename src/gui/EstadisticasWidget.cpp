#include "EstadisticasWidget.h"
#include "stats/Estadisticas.h"
#include <QBarCategoryAxis>
#include <QHorizontalStackedBarSeries>
#include <QBarSet>
#include <QChartView>
#include <QFileDialog>
#include <QGraphicsLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QResizeEvent>
#include <QSaveFile>
#include <QValueAxis>
#include <QVBoxLayout>
#include <QFrame>
#include <QString>

EstadisticasWidget::EstadisticasWidget(QWidget* parent) : QWidget(parent) {
    configurarUI();
}

void EstadisticasWidget::configurarUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(10);

    m_contenido = new QBoxLayout(QBoxLayout::TopToBottom);
    m_contenido->setSpacing(10);
    layout->addLayout(m_contenido, 1);

    auto* resumen = new QFrame;
    resumen->setObjectName("resumen");
    resumen->setStyleSheet(
        "QFrame#resumen { background: white; border: 1px solid #cbd5df; border-radius: 6px; }"
        "QLabel { color: #30485c; }");
    auto* resumenLayout = new QVBoxLayout(resumen);
    resumenLayout->setContentsMargins(12, 10, 12, 8);
    resumenLayout->setSpacing(8);

    auto* tiempoLayout = new QHBoxLayout;
    tiempoLayout->addWidget(new QLabel(tr("Tiempo promedio respuesta")));
    m_lblTiempoPromedio = new QLabel("-- s");
    m_lblTiempoPromedio->setStyleSheet("font-size:18px;font-weight:bold;color:#173751;");
    tiempoLayout->addWidget(m_lblTiempoPromedio, 1, Qt::AlignRight | Qt::AlignVCenter);
    resumenLayout->addLayout(tiempoLayout);

    m_tablaUnidades = new QTableWidget(0, 4);
    m_tablaUnidades->setHorizontalHeaderLabels({"Unidad", "Tipo", "Atendidos", "Uso %"});
    m_tablaUnidades->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tablaUnidades->setAlternatingRowColors(true);
    m_tablaUnidades->setShowGrid(false);
    m_tablaUnidades->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tablaUnidades->verticalHeader()->hide();
    m_tablaUnidades->verticalHeader()->setDefaultSectionSize(28);
    auto* cabecera = m_tablaUnidades->horizontalHeader();
    cabecera->setMinimumSectionSize(36);
    cabecera->setSectionResizeMode(QHeaderView::ResizeToContents);
    cabecera->setSectionResizeMode(1, QHeaderView::Stretch);
    m_tablaUnidades->setStyleSheet(
        "QTableWidget{background:white;alternate-background-color:#f1f5f8;color:#233b4e;"
        "border:1px solid #cbd5df;selection-background-color:#cde5f6;selection-color:#152e43;}"
        "QTableWidget::item{padding:0 3px;}"
        "QHeaderView::section{background:#e4ecf2;color:#27445b;font-weight:bold;"
        "border:none;border-bottom:1px solid #cbd5df;padding:5px 3px;}");

    m_atendidos  = new QBarSet(tr("Atendidos"));
    m_pendientes = new QBarSet(tr("Pendientes"));
    m_atendidos->setColor(QColor("#2376ad"));
    m_pendientes->setColor(QColor("#ca8420"));
    *m_atendidos  << 0 << 0;
    *m_pendientes << 0 << 0;
    auto* serie = new QHorizontalStackedBarSeries;
    serie->append(m_atendidos);
    serie->append(m_pendientes);
    serie->setBarWidth(0.45);
    auto* grafico = new QChart;
    grafico->setMargins(QMargins(0, 0, 2, 0));
    grafico->layout()->setContentsMargins(0, 0, 0, 0);
    grafico->setBackgroundRoundness(0);
    grafico->legend()->hide();
    grafico->addSeries(serie);
    m_categorias = new QBarCategoryAxis;
    m_categorias->append({tr("Pendientes: 0"), tr("Atendidos: 0")});
    m_categorias->setGridLineVisible(false);
    m_categorias->setLabelsColor(QColor("#27445b"));
    m_categorias->setTruncateLabels(false);
    grafico->addAxis(m_categorias, Qt::AlignLeft);
    serie->attachAxis(m_categorias);
    m_ejeIncidentes = new QValueAxis;
    m_ejeIncidentes->setRange(0, 1);
    m_ejeIncidentes->setVisible(false);
    grafico->addAxis(m_ejeIncidentes, Qt::AlignBottom);
    serie->attachAxis(m_ejeIncidentes);

    auto* vistaGrafico = new QChartView(grafico);
    vistaGrafico->setMinimumHeight(64);
    vistaGrafico->setMaximumHeight(120);
    vistaGrafico->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    resumenLayout->addWidget(vistaGrafico);

    auto* exportar = new QPushButton(tr("Exportar resumen"));
    exportar->setStyleSheet(
        "QPushButton{color:white;background:#245c81;border:1px solid #204d6b;"
        "border-radius:4px;font-weight:bold;padding:5px 10px;}"
        "QPushButton:hover{background:#1b4969;}");
    connect(exportar, &QPushButton::clicked, this, &EstadisticasWidget::exportarResumen);

    m_contenido->addWidget(resumen, 1);
    m_contenido->addWidget(m_tablaUnidades, 2);
    layout->addWidget(exportar, 0, Qt::AlignRight);
}

void EstadisticasWidget::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (m_contenido)
        m_contenido->setDirection(event->size().width() >= 640
            ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom);
}

void EstadisticasWidget::actualizar(const Estadisticas& stats) {
    m_lblTiempoPromedio->setText(
        tr("%1 s").arg(stats.tiempoPromedioRespuesta(), 0, 'f', 1));

    const auto& unidades = stats.datosUnidades();
    m_tablaUnidades->setRowCount(static_cast<int>(unidades.size()));
    for (int fila = 0; fila < m_tablaUnidades->rowCount(); ++fila) {
        const auto& unidad = unidades[fila];
        const QStringList valores = {
            QString::fromStdString(unidad.nombre),
            QString::fromStdString(unidad.tipo),
            QString::number(unidad.incidentesAtendidos),
            QString::number(unidad.porcentajeUso, 'f', 1) + " %"
        };
        for (int col = 0; col < valores.size(); ++col) {
            auto* celda = m_tablaUnidades->item(fila, col);
            if (!celda) {
                celda = new QTableWidgetItem;
                if (col >= 2) celda->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
                m_tablaUnidades->setItem(fila, col, celda);
            }
            celda->setText(valores[col]);
        }
    }

    m_atendidos->replace(1, stats.totalAtendidos());
    m_pendientes->replace(0, stats.totalPendientes());
    const QStringList cats = {
        tr("Pendientes: %1").arg(stats.totalPendientes()),
        tr("Atendidos: %1").arg(stats.totalAtendidos())
    };
    if (m_categorias->categories() != cats) m_categorias->setCategories(cats);
    m_ejeIncidentes->setMax(qMax(1, qMax(stats.totalAtendidos(), stats.totalPendientes())));
}

void EstadisticasWidget::limpiar() {
    actualizar(Estadisticas{});
    m_lblTiempoPromedio->setText("-- s");
}

void EstadisticasWidget::exportarResumen() {
    QStringList lineas = {
        tr("SIDEA — Resumen de estadísticas"),
        tr("Tiempo promedio: %1").arg(m_lblTiempoPromedio->text()),
        m_categorias->at(1), m_categorias->at(0), QString()
    };
    QStringList cabeceras;
    for (int col = 0; col < m_tablaUnidades->columnCount(); ++col)
        cabeceras << m_tablaUnidades->horizontalHeaderItem(col)->text();
    lineas << cabeceras.join('\t');
    for (int fila = 0; fila < m_tablaUnidades->rowCount(); ++fila) {
        QStringList valores;
        for (int col = 0; col < m_tablaUnidades->columnCount(); ++col)
            valores << m_tablaUnidades->item(fila, col)->text();
        lineas << valores.join('\t');
    }
    const QString ruta = QFileDialog::getSaveFileName(
        this, tr("Exportar resumen"), "estadisticas.txt", tr("Archivos de texto (*.txt)"));
    if (ruta.isEmpty()) return;
    QSaveFile archivo(ruta);
    const QByteArray contenido = (lineas.join('\n') + '\n').toUtf8();
    if (!archivo.open(QIODevice::WriteOnly) || archivo.write(contenido) != contenido.size()
        || !archivo.commit()) {
        QMessageBox::warning(this, tr("Error al exportar"), archivo.errorString());
    }
}
