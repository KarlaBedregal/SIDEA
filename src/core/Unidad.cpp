#include "Unidad.h"
#include "states/EstadoUnidad.h"
#include "states/EstadoLibre.h"

Unidad::~Unidad() = default;

Unidad::Unidad(int id, std::string nombre, int nodoBase)
    : m_id(id), m_nombre(std::move(nombre)), m_nodoBase(nodoBase), m_nodoActual(nodoBase),
      m_estado(std::make_unique<EstadoLibre>()) {}

void Unidad::cambiarEstado(std::unique_ptr<EstadoUnidad> nuevoEstado) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_estado = std::move(nuevoEstado);
}

void Unidad::tick() {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    if (m_estado) m_estado->manejar(*this);
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
    return m_estado && m_estado->nombre() == "Libre";
}

std::string Unidad::getEstadoNombre() const {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    return m_estado ? m_estado->nombre() : "Sin estado";
}

void Unidad::setRuta(std::vector<int> ruta) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_ruta = std::move(ruta);
}

void Unidad::asignarIncidente(std::shared_ptr<Incidente> inc) {
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_incidenteActual = std::move(inc);
}
