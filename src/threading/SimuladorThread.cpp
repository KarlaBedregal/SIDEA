#include "SimuladorThread.h"
#include "core/Unidad.h"
#include <chrono>

SimuladorThread::SimuladorThread(std::shared_ptr<Unidad> unidad,
                                  const MapaArequipa& mapa, int tickMs)
    : m_unidad(std::move(unidad)), m_mapa(mapa), m_tickMs(tickMs) {}

SimuladorThread::~SimuladorThread() { detener(); }

void SimuladorThread::iniciar() {
    if (m_activo) {
        if (m_pausado) reanudar();
        return;
    }
    m_activo = true;
    m_pausado = false;
    m_hilo = std::thread(&SimuladorThread::bucleSimulacion, this);
}

void SimuladorThread::pausar() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pausado = true;
    }
    m_cv.notify_all();
}

void SimuladorThread::reanudar() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pausado = false;
    }
    m_cv.notify_all();
}

void SimuladorThread::detener() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_activo = false;
        m_pausado = false;
    }
    m_cv.notify_all();
    if (m_hilo.joinable()) m_hilo.join();
}

bool SimuladorThread::estaActivo() const { return m_activo; }
bool SimuladorThread::estaPausado() const { return m_pausado; }

void SimuladorThread::bucleSimulacion() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_activo) {
        m_cv.wait(lock, [this]() { return !m_activo || !m_pausado; });
        if (!m_activo) break;

        if (m_cv.wait_for(lock, std::chrono::milliseconds(m_tickMs),
                          [this]() { return !m_activo || m_pausado; })) {
            continue;
        }

        if (!m_activo || m_pausado) continue;

        lock.unlock();
        m_unidad->tick();
        lock.lock();
    }
}
