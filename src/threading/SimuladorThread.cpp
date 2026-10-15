#include "SimuladorThread.h"
#include "core/Unidad.h"
#include <chrono>

SimuladorThread::SimuladorThread(std::shared_ptr<Unidad> unidad,
                                  const MapaArequipa& mapa, int tickMs)
    : m_unidad(std::move(unidad)), m_mapa(mapa), m_tickMs(tickMs) {}

SimuladorThread::~SimuladorThread() { detener(); }

void SimuladorThread::iniciar() {
    if (m_activo) return;
    m_activo = true;
    m_hilo = std::thread(&SimuladorThread::bucleSimulacion, this);
}

void SimuladorThread::detener() {
    m_activo = false;
    if (m_hilo.joinable()) m_hilo.join();
}

bool SimuladorThread::estaActivo() const { return m_activo; }

void SimuladorThread::bucleSimulacion() {
    while (m_activo) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_tickMs));
        if (!m_activo) break;
        m_unidad->tick();
    }
}
