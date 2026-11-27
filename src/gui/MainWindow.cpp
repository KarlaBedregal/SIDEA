#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MapaWidget.h"
#include "PanelControl.h"
#include "EstadisticasWidget.h"
#include "LogPanel.h"
#include "core/CentralDespacho.h"
#include "core/UnidadFactory.h"
#include "graph/MapaArequipa.h"
#include "patterns/IObserver.h"
#include "stats/Estadisticas.h"
#include "strategies/EstrategiaMasCercana.h"
#include "strategies/EstrategiaPrioridad.h"
#include "strategies/EstrategiaBalanceCarga.h"
#include "threading/GeneradorIncidentes.h"
#include <QDateTime>
#include <QHBoxLayout>
#include <QVBoxLayout>

class MainWindowObserver : public IObserver {
public:
    explicit MainWindowObserver(MainWindow* w) : m_window(w) {}
    void actualizar(const std::string& evento) override {
        QMetaObject::invokeMethod(m_window, "onEventoDespacho",
            Qt::QueuedConnection,
            Q_ARG(QString, QString::fromStdString(evento)));
    }
private:
    MainWindow* m_window;
};

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    m_mapa = std::make_unique<MapaArequipa>();
    m_mapaWidget = new MapaWidget(*m_mapa, this);

    auto obs = std::make_shared<MainWindowObserver>(this);
    m_observer = obs;
    CentralDespacho::instancia().suscribir(obs);

    m_timerSimulacion = new QTimer(this);
    connect(m_timerSimulacion, &QTimer::timeout, this, &MainWindow::onTickSimulacion);

    configurarUI();
    inicializarEscenario();
    configurarConexiones();
}

MainWindow::~MainWindow() {
    onDetenerSimulacion();
    CentralDespacho::instancia().desuscribir(m_observer);
    delete ui;
}

void MainWindow::configurarUI() {
    setWindowTitle("SIDEA — Sistema de Despacho de Emergencias de Arequipa");
    resize(1280, 800);

    auto* mapaLayout = new QVBoxLayout(ui->mapaContainer);
    mapaLayout->setContentsMargins(0, 0, 0, 0);
    mapaLayout->addWidget(m_mapaWidget);

    m_panelControl = new PanelControl(ui->panelControlContainer);
    auto* panelLayout = new QVBoxLayout(ui->panelControlContainer);
    panelLayout->setContentsMargins(0, 0, 0, 0);
    panelLayout->addWidget(m_panelControl);

    m_estadisticasWidget = new EstadisticasWidget(ui->estadisticasContainer);
    auto* statsLayout = new QVBoxLayout(ui->estadisticasContainer);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->addWidget(m_estadisticasWidget);

    // Log panel: se agrega directamente al sidePanel (el contenedor del panel derecho)
    m_logPanel = new LogPanel(this);
    m_logPanel->setMaximumHeight(220);
    auto* sideLayout = qobject_cast<QVBoxLayout*>(ui->sidePanel->layout());
    if (sideLayout) sideLayout->addWidget(m_logPanel);
}

void MainWindow::inicializarEscenario() {
    auto& central = CentralDespacho::instancia();
    if (central.getUnidades().empty()) {
        central.registrarUnidad(UnidadFactory::crear(1, TipoUnidad::AMBULANCIA, "AMB-01", 0));
        central.registrarUnidad(UnidadFactory::crear(2, TipoUnidad::AMBULANCIA, "AMB-02", 4));
        central.registrarUnidad(UnidadFactory::crear(3, TipoUnidad::AMBULANCIA, "AMB-03", 9));
        central.registrarUnidad(UnidadFactory::crear(4, TipoUnidad::BOMBERO,    "BOM-01", 1));
        central.registrarUnidad(UnidadFactory::crear(5, TipoUnidad::BOMBERO,    "BOM-02", 3));
        central.registrarUnidad(UnidadFactory::crear(6, TipoUnidad::SERENAZGO,  "SER-01", 2));
        central.registrarUnidad(UnidadFactory::crear(7, TipoUnidad::SERENAZGO,  "SER-02", 6));
        central.registrarUnidad(UnidadFactory::crear(8, TipoUnidad::SERENAZGO,  "SER-03", 8));
    }
    central.setEstrategia(std::make_unique<EstrategiaMasCercana>(*m_mapa));
    m_mapaWidget->setUnidades(central.getUnidades());
    actualizarVista();
}

void MainWindow::configurarConexiones() {
    if (!m_panelControl) return;
    connect(m_panelControl, &PanelControl::iniciarPresionado,  this, &MainWindow::onIniciarSimulacion);
    connect(m_panelControl, &PanelControl::pausarPresionado,   this, &MainWindow::onPausarSimulacion);
    connect(m_panelControl, &PanelControl::detenerPresionado,  this, &MainWindow::onDetenerSimulacion);
    connect(m_panelControl, &PanelControl::estrategiaChanged,  this, &MainWindow::onEstrategiaChanged);
    connect(m_panelControl, &PanelControl::reubicacionChanged, this, [](bool activa) {
        CentralDespacho::instancia().setReubicacionPreventiva(activa);
    });
    connect(m_panelControl, &PanelControl::reiniciarPresionado, this, [this] {
        onDetenerSimulacion();
        CentralDespacho::instancia().reiniciar();
        inicializarEscenario();
    });
}

void MainWindow::onEstrategiaChanged(int index) {
    std::unique_ptr<EstrategiaDespacho> e;
    switch (index) {
        case 1:  e = std::make_unique<EstrategiaPrioridad>(); break;
        case 2:  e = std::make_unique<EstrategiaBalanceCarga>(); break;
        default: e = std::make_unique<EstrategiaMasCercana>(*m_mapa); break;
    }
    CentralDespacho::instancia().setEstrategia(std::move(e));
}

void MainWindow::onIniciarSimulacion() {
    if (m_logPanel) m_logPanel->agregarEvento("▶ Simulación iniciada", "info");
    if (!m_simulacionIniciada) {
        const int vel = m_panelControl ? m_panelControl->velocidadSimulacion() : 500;
        if (m_panelControl) onEstrategiaChanged(m_panelControl->estrategiaSeleccionada());
        CentralDespacho::instancia().setTickMs(vel);
        CentralDespacho::instancia().iniciar();
        m_generador = std::make_unique<GeneradorIncidentes>(
            CentralDespacho::instancia(), vel * 3, m_mapa->totalNodos());
        m_generador->iniciar();
        m_timerSimulacion->start(vel);
        m_simulacionIniciada = true;
        m_simulacionPausada  = false;
        if (m_panelControl) m_panelControl->marcarIniciado();
    } else if (m_simulacionPausada) {
        if (m_generador) m_generador->reanudar();
        const int vel = m_panelControl ? m_panelControl->velocidadSimulacion() : 500;
        m_timerSimulacion->start(vel);
        m_simulacionPausada = false;
        if (m_panelControl) m_panelControl->marcarIniciado();
    }
}

void MainWindow::onPausarSimulacion() {
    if (m_simulacionIniciada && !m_simulacionPausada) {
        if (m_generador) m_generador->pausar();
        m_timerSimulacion->stop();
        m_simulacionPausada = true;
        if (m_panelControl) m_panelControl->marcarPausado();
        if (m_logPanel) m_logPanel->agregarEvento("⏸ Simulación pausada", "info");
    }
}

void MainWindow::onDetenerSimulacion() {
    if (!m_simulacionIniciada) return;
    m_timerSimulacion->stop();
    if (m_generador) { m_generador->detener(); m_generador.reset(); }
    CentralDespacho::instancia().detener();
    m_simulacionIniciada = false;
    m_simulacionPausada  = false;
    if (m_panelControl) m_panelControl->marcarDetenido();
    if (m_mapaWidget) m_mapaWidget->actualizar();
    if (m_logPanel) m_logPanel->agregarEvento("■ Simulación detenida", "info");
}

void MainWindow::onTickSimulacion() {
    auto& central = CentralDespacho::instancia();
    if (m_mapaWidget) {
        m_mapaWidget->setUnidades(central.getUnidades());
        m_mapaWidget->setIncidentes(central.getIncidentesPendientes());
        const int hora = QDateTime::currentDateTimeUtc().time().hour();
        m_mapaWidget->setZonasRiesgo(central.zonasDeMayorRiesgo(3, hora));
        m_mapaWidget->actualizar();
    }
    actualizarVista();
}

void MainWindow::onEventoDespacho(QString evento) {
    if (m_mapaWidget) {
        auto& central = CentralDespacho::instancia();
        m_mapaWidget->setUnidades(central.getUnidades());
        m_mapaWidget->setIncidentes(central.getIncidentesPendientes());
        m_mapaWidget->actualizar();
    }
    actualizarVista();

    if (m_logPanel) {
        QString tipo = "info";
        QString texto = evento;
        if (evento == "incidente_recibido") {
            tipo = "incidente";
            const int pend = CentralDespacho::instancia().totalIncidentesPendientes();
            texto = QStringLiteral("🔴 Incidente recibido — cola: %1").arg(pend);
        } else if (evento == "unidad_asignada") {
            tipo = "asignacion";
            texto = QStringLiteral("🟢 Unidad asignada al incidente");
        } else if (evento == "unidad_libre") {
            tipo = "libre";
            texto = QStringLiteral("🔵 Unidad regresó a base — disponible");
        } else if (evento == "incidente_atendido") {
            tipo = "atendido";
            const int atendidos = CentralDespacho::instancia().totalIncidentesAtendidos();
            texto = QStringLiteral("🟣 Incidente atendido — total: %1").arg(atendidos);
        }
        m_logPanel->agregarEvento(texto, tipo);
    }
}

void MainWindow::actualizarVista() {
    if (!m_estadisticasWidget) return;
    Estadisticas stats;
    auto& central = CentralDespacho::instancia();
    const int atendidos  = central.totalIncidentesAtendidos();
    const int pendientes = central.totalIncidentesPendientes();
    for (int i = 0; i < atendidos;  ++i) stats.registrarIncidenteAtendido();
    for (int i = 0; i < pendientes; ++i) stats.registrarIncidentePendiente();
    for (const auto& u : central.getUnidades()) {
        if (!u) continue;
        EstadisticaUnidad eu;
        eu.nombre = u->getNombre();
        eu.tipo   = u->getTipoUnidad();
        eu.incidentesAtendidos = u->getIncidentesAtendidos();
        eu.porcentajeUso = u->getTiempoOcupado();
        stats.agregarDatosUnidad(eu);
    }
    m_estadisticasWidget->actualizar(stats);
}
