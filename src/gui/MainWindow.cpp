#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MapaWidget.h"
#include "PanelControl.h"
#include "core/CentralDespacho.h"
#include "core/UnidadFactory.h"
#include "graph/MapaArequipa.h"
#include "patterns/IObserver.h"
#include "threading/GeneradorIncidentes.h"
#include "strategies/EstrategiaMasCercana.h"
#include "strategies/EstrategiaPrioridad.h"
#include "strategies/EstrategiaBalanceCarga.h"
#include <algorithm>
#include <QFileDialog>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QSaveFile>
#include <QTableWidget>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>

class MainWindowObserver : public IObserver {
public:
    explicit MainWindowObserver(MainWindow* w) : m_window(w) {}
    void actualizar(const std::string& evento) override {
        QMetaObject::invokeMethod(m_window, "onEventoDespacho", Qt::QueuedConnection,
            Q_ARG(QString, QString::fromStdString(evento)));
    }
private:
    MainWindow* m_window;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_mapa(std::make_unique<MapaArequipa>()) {
    ui->setupUi(this);
    m_mapaWidget = new MapaWidget(*m_mapa, this);
    m_timerSimulacion = new QTimer(this);
    m_timerSimulacion->setInterval(100);
    connect(m_timerSimulacion, &QTimer::timeout, this, &MainWindow::onTickSimulacion);
    configurarUI();
    configurarConexiones();
    inicializarEscenario();
    m_observer = std::make_shared<MainWindowObserver>(this);
    CentralDespacho::instancia().suscribir(m_observer);
    m_timerSimulacion->start();
}

MainWindow::~MainWindow() {
    onDetenerSimulacion();
    CentralDespacho::instancia().desuscribir(m_observer);
    delete ui;
}

void MainWindow::configurarUI() {
    setWindowTitle("SIDEA — Despacho de emergencias de Arequipa");
    resize(1360, 850);
    auto* mapaLayout = new QVBoxLayout(ui->mapaContainer);
    mapaLayout->setContentsMargins(0, 0, 0, 0);
    auto* leyenda = new QLabel("Unidades: ambulancia · bombero · serenazgo (número = ID)\n"
                              "Incidentes: cruz roja = urgencia · llama = incendio · "
                              "triángulo = accidente · escudo = robo", this);
    leyenda->setWordWrap(true);
    mapaLayout->addWidget(leyenda);
    mapaLayout->addWidget(m_mapaWidget, 1);
    m_panelControl = new PanelControl(ui->panelControlContainer);
    auto* controles = new QVBoxLayout(ui->panelControlContainer);
    controles->setContentsMargins(0, 0, 0, 0);
    controles->addWidget(m_panelControl);
    auto* stats = new QVBoxLayout(ui->estadisticasContainer);
    stats->setContentsMargins(0, 0, 0, 0);
    m_contadores = new QLabel(this);
    m_contadores->setWordWrap(true);
    stats->addWidget(m_contadores);
    auto* descripcion = new QLabel("Comparación por estrategia y predicción\n"
        "Respuesta: segundos desde creación hasta llegada.\n"
        "Modo registrado al asignar cada incidente.", this);
    descripcion->setWordWrap(true);
    stats->addWidget(descripcion);
    m_comparacion = new QTableWidget(0, 6, this);
    m_comparacion->setObjectName("comparacionPrediccion");
    m_comparacion->setHorizontalHeaderLabels({"Estrategia", "Pred.", "Asig.", "Lleg.", "Atend.", "Media (s)"});
    m_comparacion->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_comparacion->verticalHeader()->hide();
    m_comparacion->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    m_comparacion->horizontalHeader()->setStretchLastSection(true);
    m_comparacion->setMinimumWidth(440);
    stats->addWidget(m_comparacion);
    auto* exportar = new QPushButton("Exportar comparación CSV", this);
    connect(exportar, &QPushButton::clicked, this, &MainWindow::exportarComparacion);
    stats->addWidget(exportar);
    auto* layout = qobject_cast<QHBoxLayout*>(ui->centralwidget->layout());
    if (layout) { layout->setStretch(0, 3); layout->setStretch(1, 2); }
    auto* lateral = qobject_cast<QVBoxLayout*>(ui->sidePanel->layout());
    if (lateral) { lateral->setStretch(0, 0); lateral->setStretch(1, 1); }
}

void MainWindow::configurarConexiones() {
    connect(m_panelControl, &PanelControl::iniciarPresionado, this, &MainWindow::onIniciarSimulacion);
    connect(m_panelControl, &PanelControl::pausarPresionado, this, &MainWindow::onPausarSimulacion);
    connect(m_panelControl, &PanelControl::detenerPresionado, this, &MainWindow::onDetenerSimulacion);
    connect(m_panelControl, &PanelControl::estrategiaChanged, this, &MainWindow::onEstrategiaChanged);
    connect(m_panelControl, &PanelControl::reubicacionChanged, this, [](bool activa) {
        CentralDespacho::instancia().setReubicacionPreventiva(activa);
    });
    connect(m_panelControl, &PanelControl::reiniciarPresionado, this, [this] {
        onDetenerSimulacion();
        CentralDespacho::instancia().reiniciar();
        m_generador.reset();
        inicializarEscenario();
    });
}

void MainWindow::inicializarEscenario() {
    auto& central = CentralDespacho::instancia();
    if (central.getUnidades().empty()) {
        central.registrarUnidad(UnidadFactory::crear(1, TipoUnidad::AMBULANCIA, "AMB-01", 0));
        central.registrarUnidad(UnidadFactory::crear(2, TipoUnidad::AMBULANCIA, "AMB-02", 4));
        central.registrarUnidad(UnidadFactory::crear(3, TipoUnidad::AMBULANCIA, "AMB-03", 9));
        central.registrarUnidad(UnidadFactory::crear(4, TipoUnidad::BOMBERO, "BOM-01", 1));
        central.registrarUnidad(UnidadFactory::crear(5, TipoUnidad::BOMBERO, "BOM-02", 3));
        central.registrarUnidad(UnidadFactory::crear(6, TipoUnidad::SERENAZGO, "SER-01", 2));
        central.registrarUnidad(UnidadFactory::crear(7, TipoUnidad::SERENAZGO, "SER-02", 6));
        central.registrarUnidad(UnidadFactory::crear(8, TipoUnidad::SERENAZGO, "SER-03", 8));
    }
    onEstrategiaChanged(m_panelControl->estrategiaSeleccionada());
    onTickSimulacion();
}

void MainWindow::onIniciarSimulacion() {
    if (m_enEjecucion) return;
    auto& central = CentralDespacho::instancia();
    const int tick = m_panelControl->velocidadSimulacion();
    central.setTickMs(tick);
    m_mapaWidget->setDuracionMovimiento(tick);
    if (!m_generador)
        m_generador = std::make_unique<GeneradorIncidentes>(central, 2000, m_mapa->totalNodos() - 1);
    // La central es la única dueña de los hilos de unidad y del consumidor.
    central.iniciar();
    m_generador->iniciar();
    m_enEjecucion = true;
    m_panelControl->marcarIniciado();
}

void MainWindow::onPausarSimulacion() {
    if (!m_enEjecucion) return;
    if (m_generador) m_generador->detener();
    CentralDespacho::instancia().detener();
    m_enEjecucion = false;
    m_panelControl->marcarPausado();
    onTickSimulacion();
}

void MainWindow::onDetenerSimulacion() {
    if (m_generador) m_generador->detener();
    CentralDespacho::instancia().detener();
    m_enEjecucion = false;
    if (m_panelControl) m_panelControl->marcarDetenido();
}

void MainWindow::onEstrategiaChanged(int index) {
    auto& central = CentralDespacho::instancia();
    switch (index) {
        case 1: central.setEstrategia(std::make_unique<EstrategiaPrioridad>()); break;
        case 2: central.setEstrategia(std::make_unique<EstrategiaBalanceCarga>()); break;
        default: central.setEstrategia(std::make_unique<EstrategiaMasCercana>(*m_mapa)); break;
    }
}

void MainWindow::onTickSimulacion() {
    auto& central = CentralDespacho::instancia();
    if (m_enEjecucion) central.reubicarUnidades();
    m_mapaWidget->setUnidades(central.getUnidades());
    // Los pendientes forman parte de los activos; los asignados permanecen hasta ser atendidos.
    auto incidentes = central.getIncidentesPendientes();
    for (const auto& activo : central.getIncidentesActivos()) {
        if (std::find(incidentes.begin(), incidentes.end(), activo) == incidentes.end())
            incidentes.push_back(activo);
    }
    m_mapaWidget->setIncidentes(incidentes);
    actualizarComparacion();
}

void MainWindow::onEventoDespacho(QString evento) {
    (void)evento;
    onTickSimulacion();
}

void MainWindow::actualizarComparacion() {
    const auto& central = CentralDespacho::instancia();
    const int pendientes = central.totalIncidentesPendientes();
    const int atendidos = central.totalIncidentesAtendidos();
    const int activos = static_cast<int>(central.getIncidentesActivos().size());
    m_contadores->setText(QString("Recibidos: %1 · En cola: %2\nEn servicio: %3 · Atendidos: %4")
        .arg(activos + atendidos).arg(pendientes).arg(activos - pendientes).arg(atendidos));
    const auto resumenes = central.comparacionPrediccion();
    m_comparacion->setRowCount(static_cast<int>(resumenes.size()));
    int fila = 0;
    for (const auto& r : resumenes) {
        const QStringList valores{QString::fromStdString(r.estrategia), r.prediccion ? "Sí" : "No",
            QString::number(r.asignados), QString::number(r.llegadas), QString::number(r.atendidos),
            r.llegadas ? QString::number(r.respuestaMediaSegundos, 'f', 2) : "—"};
        for (int columna = 0; columna < valores.size(); ++columna)
            m_comparacion->setItem(fila, columna, new QTableWidgetItem(valores[columna]));
        ++fila;
    }
}

void MainWindow::exportarComparacion() {
    const QString ruta = QFileDialog::getSaveFileName(this, "Exportar comparación", "comparacion.csv", "CSV (*.csv)");
    if (ruta.isEmpty()) return;
    QSaveFile archivo(ruta);
    if (!archivo.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Exportar", archivo.errorString());
        return;
    }
    QTextStream salida(&archivo);
    salida << "estrategia,prediccion,asignados,llegadas,atendidos,respuesta_media_s\n";
    for (const auto& r : CentralDespacho::instancia().comparacionPrediccion()) {
        salida << QString::fromStdString(r.estrategia) << ',' << (r.prediccion ? "si" : "no")
               << ',' << r.asignados << ',' << r.llegadas << ',' << r.atendidos << ',';
        if (r.llegadas) salida << QString::number(r.respuestaMediaSegundos, 'f', 6);
        salida << '\n';
    }
    salida.flush();
    if (!archivo.commit()) QMessageBox::warning(this, "Exportar", archivo.errorString());
}
