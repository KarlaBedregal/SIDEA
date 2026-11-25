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
    std::lock_guard<std::mutex> ciclo(m_cicloMutex);
    if (m_activo) return;
    if (m_hilo.joinable()) m_hilo.join();
    {
        std::lock_guard<std::mutex> lock(m_mutexEspera);
        m_activo = true;
        m_generados = 0;
    }
    m_hilo = std::thread(&GeneradorIncidentes::bucleGeneracion, this);
}

void GeneradorIncidentes::detener() {
    std::lock_guard<std::mutex> ciclo(m_cicloMutex);
    {
        std::lock_guard<std::mutex> lock(m_mutexEspera);
        m_activo = false;
    }
    m_cvParada.notify_all();
    if (m_hilo.joinable()) m_hilo.join();
}

bool GeneradorIncidentes::estaActivo() const { return m_activo; }
int GeneradorIncidentes::totalGenerados() const { return m_generados; }

void GeneradorIncidentes::bucleGeneracion() {
    std::unique_lock<std::mutex> lock(m_mutexEspera);
    while (m_activo && (m_limiteIncidentes == 0 || m_generados < m_limiteIncidentes)) {
        if (m_cvParada.wait_for(lock, std::chrono::milliseconds(m_intervaloMs),
                              [&] { return !m_activo; })) break;
        lock.unlock();
        auto inc = IncidenteFactory::crearAleatorio(m_contadorId++, m_nodoMax);
        m_central.recibirIncidente(inc);
        ++m_generados;
        lock.lock();
    }
    m_activo = false;
}
