#pragma once
#include <QMainWindow>
#include <memory>

class MapaWidget;
class PanelControl;
class EstadisticasWidget;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

signals:
    void simulacionIniciada();
    void simulacionPausada();
    void simulacionDetenida();
    void estrategiaCambiada(int index);
    void tickSimulacion();

private:
    void configurarUI();
    void configurarConexiones();

    Ui::MainWindow* ui;
    MapaWidget* m_mapaWidget;
    PanelControl* m_panelControl;
    EstadisticasWidget* m_estadisticasWidget;
};
