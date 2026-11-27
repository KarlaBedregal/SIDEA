#pragma once
#include <QMainWindow>
#include <memory>

class MapaWidget;
class PanelControl;
class MapaArequipa;
class IObserver;
class QTimer;
class QTableWidget;
class QLabel;
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
    void actualizarComparacion();
    void exportarComparacion();

    Ui::MainWindow* ui;
    std::unique_ptr<MapaArequipa> m_mapa;
    MapaWidget* m_mapaWidget = nullptr;
    PanelControl* m_panelControl = nullptr;
    std::shared_ptr<IObserver> m_observer;
    std::unique_ptr<GeneradorIncidentes> m_generador;
    QTimer* m_timerSimulacion = nullptr;
    QTableWidget* m_comparacion = nullptr;
    QLabel* m_contadores = nullptr;
    bool m_enEjecucion = false;
};
