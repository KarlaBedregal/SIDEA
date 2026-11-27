#pragma once
#include <QMainWindow>
#include <QTimer>
#include <memory>
#include <vector>

class MapaWidget;
class PanelControl;
class MapaArequipa;
class IObserver;
class SimuladorThread;
class GeneradorIncidentes;

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
    void onEventoDespacho(QString evento);

private:
    void configurarUI();
    void configurarConexiones();
    void inicializarEscenario();
    void actualizarEstadisticas();

    Ui::MainWindow* ui;
    std::unique_ptr<MapaArequipa> m_mapa;
    MapaWidget* m_mapaWidget = nullptr;
    PanelControl* m_panelControl = nullptr;
    std::shared_ptr<IObserver> m_observer;

    QTimer* m_timerSimulacion = nullptr;
    bool m_simulacionIniciada = false;
    bool m_simulacionPausada = false;
};
