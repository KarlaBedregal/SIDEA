#include "GeneradorIncidentes.h"
#include "core/CentralDespacho.h"
#include "core/IncidenteFactory.h"
#include <chrono>

GeneradorIncidentes::GeneradorIncidentes(CentralDespacho& central, int intervaloMs, int nodoMax)
    : m_central(central), m_intervaloMs(intervaloMs), m_nodoMax(nodoMax) {}

GeneradorIncidentes::~GeneradorIncidentes() { detener(); }

void GeneradorIncidentes::iniciar() {
    if (m_activo) return;
    m_activo = true;
    m_hilo = std::thread(&GeneradorIncidentes::bucleGeneracion, this);
}

void GeneradorIncidentes::detener() {
    m_activo = false;
    if (m_hilo.joinable()) m_hilo.join();
}

bool GeneradorIncidentes::estaActivo() const { return m_activo; }

void GeneradorIncidentes::bucleGeneracion() {
    while (m_activo) {
        std::this_thread::sleep_for(std::chrono::milliseconds(m_intervaloMs));
        if (!m_activo) break;

        auto inc = IncidenteFactory::crearAleatorio(m_contadorId++, m_nodoMax);
        m_central.recibirIncidente(inc);
    }
}
