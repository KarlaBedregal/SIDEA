#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MapaWidget.h"
#include "PanelControl.h"
#include "EstadisticasWidget.h"
#include "core/CentralDespacho.h"
#include "graph/MapaArequipa.h"
#include "patterns/IObserver.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

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

    configurarUI();
    configurarConexiones();
}

MainWindow::~MainWindow() {
    CentralDespacho::instancia().desuscribir(m_observer);
    delete ui;
}

void MainWindow::configurarUI() {
    setWindowTitle("SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa");
    resize(1280, 800);

    auto* mapaLayout = new QVBoxLayout(ui->mapaContainer);
    mapaLayout->setContentsMargins(0, 0, 0, 0);
    mapaLayout->addWidget(m_mapaWidget);
    m_mapaWidget->setUnidades(CentralDespacho::instancia().getUnidades());
}

void MainWindow::configurarConexiones() {
    // TODO Sprint 3: conectar signals de PanelControl con slots de MainWindow
}
