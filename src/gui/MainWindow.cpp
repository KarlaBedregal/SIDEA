#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MapaWidget.h"
#include "PanelControl.h"
#include "EstadisticasWidget.h"
#include "core/CentralDespacho.h"
#include "core/UnidadFactory.h"
#include "graph/MapaArequipa.h"
#include "patterns/IObserver.h"
#include "threading/SimuladorThread.h"
#include "threading/GeneradorIncidentes.h"
#include "strategies/EstrategiaMasCercana.h"
#include "strategies/EstrategiaPrioridad.h"
#include "strategies/EstrategiaBalanceCarga.h"
#include "stats/Estadisticas.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

// Adapter: recibe actualizar() desde cualquier hilo y lo encola al hilo Qt
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
    setWindowTitle("SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa");
    resize(1280, 800);

    // 1. Integrar MapaWidget en mapaContainer
    auto* mapaLayout = new QVBoxLayout(ui->mapaContainer);
    mapaLayout->setContentsMargins(0, 0, 0, 0);
    mapaLayout->addWidget(m_mapaWidget);

    // 2. Crear e integrar PanelControl en panelControlContainer
    m_panelControl = new PanelControl(ui->panelControlContainer);
    auto* panelCtrlLayout = new QVBoxLayout(ui->panelControlContainer);
    panelCtrlLayout->setContentsMargins(0, 0, 0, 0);
    panelCtrlLayout->addWidget(m_panelControl);

    // 3. Crear e integrar EstadisticasWidget en estadisticasContainer
    m_estadisticasWidget = new EstadisticasWidget(ui->estadisticasContainer);
    auto* statsLayout = new QVBoxLayout(ui->estadisticasContainer);
    statsLayout->setContentsMargins(0, 0, 0, 0);
    statsLayout->addWidget(m_estadisticasWidget);

    // 4. Distribuir el espacio horizontal de forma responsiva (Mapa ~75%, Panel ~25%)
    auto* mainHLayout = qobject_cast<QHBoxLayout*>(ui->centralwidget->layout());
    if (mainHLayout) {
        mainHLayout->setStretch(0, 3);
        mainHLayout->setStretch(1, 1);
    }
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

    m_mapaWidget->setUnidades(central.getUnidades());

    if (m_panelControl) {
        onEstrategiaChanged(m_panelControl->estrategiaSeleccionada());
    } else {
        central.setEstrategia(std::make_unique<EstrategiaMasCercana>(*m_mapa));
    }

    actualizarEstadisticas();
    m_mapaWidget->actualizar();
}

void MainWindow::configurarConexiones() {
    if (!m_panelControl) return;

    connect(m_panelControl, &PanelControl::iniciarPresionado,
            this, &MainWindow::onIniciarSimulacion);
    connect(m_panelControl, &PanelControl::pausarPresionado,
            this, &MainWindow::onPausarSimulacion);
    connect(m_panelControl, &PanelControl::detenerPresionado,
            this, &MainWindow::onDetenerSimulacion);
    connect(m_panelControl, &PanelControl::estrategiaChanged,
            this, &MainWindow::onEstrategiaChanged);
}

void MainWindow::onEstrategiaChanged(int index) {
    std::unique_ptr<EstrategiaDespacho> estrategia;
    switch (index) {
        case 0:
            estrategia = std::make_unique<EstrategiaMasCercana>(*m_mapa);
            break;
        case 1:
            estrategia = std::make_unique<EstrategiaPrioridad>();
            break;
        case 2:
            estrategia = std::make_unique<EstrategiaBalanceCarga>();
            break;
        default:
            estrategia = std::make_unique<EstrategiaMasCercana>(*m_mapa);
            break;
    }
    CentralDespacho::instancia().setEstrategia(std::move(estrategia));
}

void MainWindow::onIniciarSimulacion() {
    if (!m_simulacionIniciada) {
        int vel = m_panelControl ? m_panelControl->velocidadSimulacion() : 500;
        if (m_panelControl) {
            onEstrategiaChanged(m_panelControl->estrategiaSeleccionada());
        }

        m_simuladores.clear();
        for (const auto& u : CentralDespacho::instancia().getUnidades()) {
            auto sim = std::make_unique<SimuladorThread>(u, *m_mapa, vel);
            sim->iniciar();
            m_simuladores.push_back(std::move(sim));
        }

        m_generador = std::make_unique<GeneradorIncidentes>(
            CentralDespacho::instancia(), vel * 2, m_mapa->totalNodos());
        m_generador->iniciar();

        if (m_timerSimulacion) m_timerSimulacion->start(vel);
        m_simulacionIniciada = true;
        m_simulacionPausada = false;
        if (m_panelControl) m_panelControl->marcarIniciado();
    } else if (m_simulacionPausada) {
        for (auto& sim : m_simuladores) {
            if (sim) sim->reanudar();
        }
        if (m_generador) m_generador->reanudar();

        int vel = m_panelControl ? m_panelControl->velocidadSimulacion() : 500;
        if (m_timerSimulacion) m_timerSimulacion->start(vel);
        m_simulacionPausada = false;
        if (m_panelControl) m_panelControl->marcarIniciado();
    }
}

void MainWindow::onPausarSimulacion() {
    if (m_simulacionIniciada && !m_simulacionPausada) {
        for (auto& sim : m_simuladores) {
            if (sim) sim->pausar();
        }
        if (m_generador) m_generador->pausar();

        if (m_timerSimulacion) m_timerSimulacion->stop();
        m_simulacionPausada = true;
        if (m_panelControl) m_panelControl->marcarPausado();
    }
}

void MainWindow::onDetenerSimulacion() {
    if (!m_simulacionIniciada) return;

    if (m_timerSimulacion) m_timerSimulacion->stop();

    if (m_generador) {
        m_generador->detener();
        m_generador.reset();
    }

    for (auto& sim : m_simuladores) {
        if (sim) sim->detener();
    }
    m_simuladores.clear();

    CentralDespacho::instancia().detener();

    m_simulacionIniciada = false;
    m_simulacionPausada = false;
    if (m_panelControl) m_panelControl->marcarDetenido();
    if (m_mapaWidget) m_mapaWidget->actualizar();
}

void MainWindow::onTickSimulacion() {
    CentralDespacho::instancia().procesarCola();
    if (m_mapaWidget) {
        m_mapaWidget->setUnidades(CentralDespacho::instancia().getUnidades());
        m_mapaWidget->actualizar();
    }
    actualizarEstadisticas();
}

void MainWindow::onEventoDespacho(QString evento) {
    CentralDespacho::instancia().procesarCola();
    if (m_mapaWidget) {
        m_mapaWidget->setUnidades(CentralDespacho::instancia().getUnidades());
        m_mapaWidget->actualizar();
    }
    actualizarEstadisticas();
    (void)evento;
}

void MainWindow::actualizarEstadisticas() {
    if (!m_estadisticasWidget) return;
    Estadisticas stats;
    int atendidos = CentralDespacho::instancia().totalIncidentesAtendidos();
    int pendientes = CentralDespacho::instancia().totalIncidentesPendientes();
    for (int i = 0; i < atendidos; ++i) stats.registrarIncidenteAtendido();
    for (int i = 0; i < pendientes; ++i) stats.registrarIncidentePendiente();

    for (const auto& u : CentralDespacho::instancia().getUnidades()) {
        if (!u) continue;
        EstadisticaUnidad eu;
        eu.nombre = u->getNombre();
        eu.tipo = u->getTipoUnidad();
        eu.incidentesAtendidos = u->getIncidentesAtendidos();
        eu.porcentajeUso = u->getTiempoOcupado();
        stats.agregarDatosUnidad(eu);
    }
    m_estadisticasWidget->actualizar(stats);
}
