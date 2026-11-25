#include "Unidad.h"
#include "states/EstadoUnidad.h"
#include "states/EstadoLibre.h"
#include "CentralDespacho.h"
#include "Incidente.h"
#include "graph/Dijkstra.h"
#include "graph/MapaArequipa.h"
#include <stdexcept>

namespace {
std::vector<int> rutaEntre(const MapaArequipa& mapa, int origen, int destino) {
    const auto& grafo = mapa.grafo();
    if (!grafo.existeNodo(origen) || !grafo.existeNodo(destino)) return {};
    auto resultado = dijkstra(grafo, origen);
    return resultado.alcanzable(destino) ? resultado.reconstruirRuta(destino)
                                        : std::vector<int>{};
}
}

Unidad::~Unidad() = default;

Unidad::Unidad(int id, std::string nombre, int nodoBase)
    : m_mapa(std::make_unique<MapaArequipa>()),
      m_id(id), m_nombre(std::move(nombre)), m_nodoBase(nodoBase), m_nodoActual(nodoBase),
      m_estado(std::make_unique<EstadoLibre>()) {}

void Unidad::cambiarEstado(std::unique_ptr<EstadoUnidad> nuevoEstado) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (!nuevoEstado) throw std::invalid_argument("El estado no puede ser nulo");
    // No destruir el estado mientras su manejar() todavía está ejecutándose.
    if (m_enTick) m_estadoPendiente = std::move(nuevoEstado);
    else m_estado = std::move(nuevoEstado);
}

void Unidad::tick() {
    bool acabaDeLiberarse = false;
    {
        std::lock_guard<std::recursive_mutex> lock(m_mutex);
        const bool estabaLibre = estaLibre();
        m_enTick = true;
        try {
            if (m_estado) m_estado->manejar(*this);
        } catch (...) {
            m_enTick = false;
            m_estadoPendiente.reset();
            throw;
        }
        m_enTick = false;
        if (m_estadoPendiente) m_estado = std::move(m_estadoPendiente);
        acabaDeLiberarse = !estabaLibre && estaLibre();
    }
    // La central puede tomar su propio mutex: avisar después de soltar el de Unidad.
    if (acabaDeLiberarse) CentralDespacho::instancia().notificarUnidadLibre();
}

void Unidad::avanzarNodo(int nodo) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_nodoActual = nodo;
}

// m_id, m_nombre, m_nodoBase se fijan en el constructor y nunca cambian — no requieren lock.
int Unidad::getId() const { return m_id; }
std::string Unidad::getNombre() const { return m_nombre; }
int Unidad::getNodoBase() const { return m_nodoBase; }

int Unidad::getNodoActual() const {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return m_nodoActual;
}

int Unidad::getIncidentesAtendidos() const {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return m_incidentesAtendidos;
}

double Unidad::getTiempoOcupado() const {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return m_tiempoOcupado;
}

bool Unidad::estaLibre() const {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return m_estado && m_estado->nombre() == "Libre" && !m_incidenteActual;
}

std::string Unidad::getEstadoNombre() const {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return m_estado ? m_estado->nombre() : "Sin estado";
}

void Unidad::setRuta(std::vector<int> ruta) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_ruta = std::move(ruta);
    m_indiceRuta = !m_ruta.empty() && m_ruta.front() == m_nodoActual ? 1 : 0;
}

bool Unidad::asignarIncidente(std::shared_ptr<Incidente> inc) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (!estaLibre() || !inc || inc->estaAtendido()) return false;
    const auto prioridad = static_cast<int>(inc->getPrioridad());
    if (!puedeAtender(*inc) || prioridad < 1 || prioridad > 4) return false;
    if (rutaEntre(*m_mapa, m_nodoActual, inc->getNodoUbicacion()).empty() ||
        rutaEntre(*m_mapa, inc->getNodoUbicacion(), m_nodoBase).empty()) return false;
    m_incidenteActual = std::move(inc);
    m_inicioOcupacion = std::chrono::steady_clock::now();
    return true;
}

bool Unidad::puedeAtender(const Incidente& inc) const {
    const auto tipo = getTipoUnidad();
    switch (inc.getTipo()) {
        case TipoIncidente::URGENCIA_MEDICA: return tipo == "Ambulancia";
        case TipoIncidente::INCENDIO:
        case TipoIncidente::ACCIDENTE: return tipo == "Bombero";
        case TipoIncidente::ROBO: return tipo == "Serenazgo";
    }
    return false;
}

bool Unidad::prepararRuta(int destino) {
    auto ruta = rutaEntre(*m_mapa, m_nodoActual, destino);
    if (ruta.empty()) return false;
    setRuta(std::move(ruta));
    return true;
}

bool Unidad::avanzarEnRuta() {
    if (m_indiceRuta < m_ruta.size()) m_nodoActual = m_ruta[m_indiceRuta++];
    return !m_ruta.empty() && m_nodoActual == m_ruta.back();
}
