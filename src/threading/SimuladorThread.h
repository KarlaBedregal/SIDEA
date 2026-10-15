#pragma once
#include <thread>
#include <atomic>
#include <memory>

class Unidad;
class MapaArequipa;

class SimuladorThread {
public:
    SimuladorThread(std::shared_ptr<Unidad> unidad,
                    const MapaArequipa& mapa,
                    int tickMs = 500);
    ~SimuladorThread();

    void iniciar();
    void detener();
    bool estaActivo() const;

private:
    void bucleSimulacion();

    std::shared_ptr<Unidad> m_unidad;
    const MapaArequipa& m_mapa;
    int m_tickMs;
    std::thread m_hilo;
    std::atomic<bool> m_activo{false};
};
