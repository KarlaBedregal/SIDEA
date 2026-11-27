#pragma once
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>

class Unidad;
class MapaArequipa;

class SimuladorThread {
public:
    SimuladorThread(std::shared_ptr<Unidad> unidad,
                    const MapaArequipa& mapa,
                    int tickMs = 500);
    ~SimuladorThread();

    void iniciar();
    void pausar();
    void reanudar();
    void detener();
    bool estaActivo() const;
    bool estaPausado() const;
    const std::shared_ptr<Unidad>& unidad() const;

private:
    void bucleSimulacion();

    std::shared_ptr<Unidad> m_unidad;
    const MapaArequipa& m_mapa;
    int m_tickMs;
    std::thread m_hilo;
    std::atomic<bool> m_activo{false};
    std::atomic<bool> m_pausado{false};
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
};
