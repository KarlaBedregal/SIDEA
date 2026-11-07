#include "Unidad.h"
#include "states/EstadoUnidad.h"
#include "states/EstadoLibre.h"

Unidad::~Unidad() = default;

Unidad::Unidad(int id, std::string nombre, int nodoBase)
    : m_id(id), m_nombre(std::move(nombre)), m_nodoBase(nodoBase), m_nodoActual(nodoBase),
      m_estado(std::make_unique<EstadoLibre>()) {}

void Unidad::cambiarEstado(std::unique_ptr<EstadoUnidad> nuevoEstado) {
    m_estado = std::move(nuevoEstado);
}

void Unidad::tick() {
    if (m_estado) m_estado->manejar(*this);
}

int Unidad::getId() const { return m_id; }
std::string Unidad::getNombre() const { return m_nombre; }
int Unidad::getNodoActual() const { return m_nodoActual; }
int Unidad::getNodoBase() const { return m_nodoBase; }
int Unidad::getIncidentesAtendidos() const { return m_incidentesAtendidos; }
double Unidad::getTiempoOcupado() const { return m_tiempoOcupado; }

bool Unidad::estaLibre() const {
    return m_estado && m_estado->nombre() == "Libre";
}

std::string Unidad::getEstadoNombre() const {
    return m_estado ? m_estado->nombre() : "Sin estado";
}

void Unidad::setRuta(std::vector<int> ruta) {
    m_ruta = std::move(ruta);
}

void Unidad::asignarIncidente(std::shared_ptr<Incidente> inc) {
    m_incidenteActual = std::move(inc);
}
