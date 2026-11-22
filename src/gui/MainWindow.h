#pragma once
#include <QMainWindow>
#include <memory>

class MapaWidget;
class PanelControl;
class EstadisticasWidget;
class MapaArequipa;
class IObserver;

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

    Ui::MainWindow* ui;
    std::unique_ptr<MapaArequipa> m_mapa;
    MapaWidget* m_mapaWidget = nullptr;
    PanelControl* m_panelControl = nullptr;
    EstadisticasWidget* m_estadisticasWidget = nullptr;
    std::shared_ptr<IObserver> m_observer;
};
