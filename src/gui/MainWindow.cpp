#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MapaWidget.h"
#include "PanelControl.h"
#include "EstadisticasWidget.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    configurarUI();
    configurarConexiones();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::configurarUI() {
    setWindowTitle("SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa");
    resize(1280, 800);

    // El panel de control no estaba creado: se coloca en su contenedor del .ui
    m_panelControl = new PanelControl(this);
    auto* layout = new QVBoxLayout(ui->panelControlContainer);
    layout->addWidget(m_panelControl);
}

void MainWindow::configurarConexiones() {
    connect(m_panelControl, &PanelControl::iniciarPresionado, this, &MainWindow::simulacionIniciada);
    connect(m_panelControl, &PanelControl::pausarPresionado, this, &MainWindow::simulacionPausada);
    connect(m_panelControl, &PanelControl::detenerPresionado, this, &MainWindow::simulacionDetenida);
    connect(m_panelControl, &PanelControl::estrategiaChanged, this, &MainWindow::estrategiaCambiada);
}