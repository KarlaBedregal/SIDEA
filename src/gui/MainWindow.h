#pragma once
#include <QMainWindow>
#include <memory>

class MapaWidget;
class PanelControl;
class EstadisticasWidget;
class CentralDespacho;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onIniciarSimulacion();
    void onPausarSimulacion();
    void onDetenerSimulacion();
    void onEstrategiaChanged(int index);
    void onTickSimulacion();

private:
    void configurarUI();
    void configurarConexiones();

    Ui::MainWindow* ui;
    MapaWidget* m_mapaWidget;
    PanelControl* m_panelControl;
    EstadisticasWidget* m_estadisticasWidget;
};
