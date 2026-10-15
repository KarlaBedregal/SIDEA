#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MapaWidget.h"
#include "PanelControl.h"
#include "EstadisticasWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QSplitter>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    configurarUI();
    configurarConexiones();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::configurarUI() {
    // TODO Sprint 3: ensamblar widgets en el layout principal
    setWindowTitle("SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa");
    resize(1280, 800);
}

void MainWindow::configurarConexiones() {
    // TODO Sprint 3: conectar signals de PanelControl con slots de MainWindow
}

void MainWindow::onIniciarSimulacion() {}
void MainWindow::onPausarSimulacion() {}
void MainWindow::onDetenerSimulacion() {}
void MainWindow::onEstrategiaChanged(int index) {}
void MainWindow::onTickSimulacion() {}
