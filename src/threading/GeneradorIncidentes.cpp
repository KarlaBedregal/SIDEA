#include "GeneradorIncidentes.h"
#include "core/CentralDespacho.h"
#include "core/IncidenteFactory.h"
#include <chrono>

GeneradorIncidentes::GeneradorIncidentes(CentralDespacho& central, int intervaloMs, int nodoMax)
    : m_central(central), m_intervaloMs(intervaloMs), m_nodoMax(nodoMax) {}

GeneradorIncidentes::~GeneradorIncidentes() { detener(); }

void GeneradorIncidentes::iniciar() {
    if (m_activo) {
        if (m_pausado) reanudar();
        return;
    }
    m_activo = true;
    m_pausado = false;
    m_hilo = std::thread(&GeneradorIncidentes::bucleGeneracion, this);
}

void GeneradorIncidentes::pausar() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pausado = true;
    }
    m_cv.notify_all();
}

void GeneradorIncidentes::reanudar() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_pausado = false;
    }
    m_cv.notify_all();
}

void GeneradorIncidentes::detener() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_activo = false;
        m_pausado = false;
    }
    m_cv.notify_all();
    if (m_hilo.joinable()) m_hilo.join();
}

bool GeneradorIncidentes::estaActivo() const { return m_activo; }
bool GeneradorIncidentes::estaPausado() const { return m_pausado; }

void GeneradorIncidentes::bucleGeneracion() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_activo) {
        m_cv.wait(lock, [this]() { return !m_activo || !m_pausado; });
        if (!m_activo) break;

        if (m_cv.wait_for(lock, std::chrono::milliseconds(m_intervaloMs),
                          [this]() { return !m_activo || m_pausado; })) {
            continue;
        }

        if (!m_activo || m_pausado) continue;

        auto inc = IncidenteFactory::crearAleatorio(m_contadorId++, m_nodoMax);
        lock.unlock();
        m_central.recibirIncidente(inc);
        lock.lock();
    }
}
