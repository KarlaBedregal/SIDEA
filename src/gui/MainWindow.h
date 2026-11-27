#pragma once
#include <QMainWindow>
#include <QTimer>
#include <memory>

class MapaWidget;
class PanelControl;
class EstadisticasWidget;
class LogPanel;
class MapaArequipa;
class IObserver;
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
    void onGestionarUnidades();
    void onCantidadTipoCambiada(int tipoIndex, int nuevaCantidad);
    void onEstrategiaChanged(int index);
    void onTickSimulacion();
    void onEventoDespacho(QString evento);

private:
    void configurarUI();
    void configurarConexiones();
    void inicializarEscenario();
    void sincronizarConteoUnidades();
    void actualizarVista();

    Ui::MainWindow* ui;
    std::unique_ptr<MapaArequipa> m_mapa;
    MapaWidget* m_mapaWidget = nullptr;
    PanelControl* m_panelControl = nullptr;
    EstadisticasWidget* m_estadisticasWidget = nullptr;
    LogPanel* m_logPanel = nullptr;
    std::shared_ptr<IObserver> m_observer;

    std::unique_ptr<GeneradorIncidentes> m_generador;
    QTimer* m_timerSimulacion = nullptr;
    bool m_simulacionIniciada = false;
    bool m_simulacionPausada = false;
};
