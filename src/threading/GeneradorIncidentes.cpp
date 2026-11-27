#include "GeneradorIncidentes.h"
#include "core/CentralDespacho.h"
#include "core/IncidenteFactory.h"
#include <chrono>
#include "graph/MapaArequipa.h"
#include <algorithm>
#include <stdexcept>

GeneradorIncidentes::GeneradorIncidentes(CentralDespacho& central, int intervaloMs,
                                       int nodoMax, int limiteIncidentes)
    : m_central(central), m_intervaloMs(intervaloMs), m_nodoMax(nodoMax),
      m_limiteIncidentes(limiteIncidentes) {
    if (intervaloMs <= 0 || nodoMax < 0 || limiteIncidentes < 0)
        throw std::invalid_argument("Parámetros no válidos para el generador");
    // nodoMax es inclusivo, como en la fábrica; limitarlo a las zonas reales.
    static const MapaArequipa mapa;
    m_nodoMax = std::min(nodoMax, mapa.totalNodos() - 1);
}

GeneradorIncidentes::~GeneradorIncidentes() { detener(); }

void GeneradorIncidentes::iniciar() {
    if (m_activo) {
        if (m_pausado) reanudar();
        return;
    }
    if (m_hilo.joinable()) m_hilo.join();
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
int GeneradorIncidentes::totalGenerados() const { return m_contadorId.load(); }

void GeneradorIncidentes::bucleGeneracion() {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_activo) {
        if (m_limiteIncidentes > 0 && m_contadorId.load() >= m_limiteIncidentes)
            break;
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
        if (m_limiteIncidentes > 0 && m_contadorId.load() >= m_limiteIncidentes)
            m_activo = false;
    }
    m_activo = false;
}
