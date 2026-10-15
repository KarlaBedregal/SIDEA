#include "Serenazgo.h"
#include "Incidente.h"

Serenazgo::Serenazgo(int id, std::string nombre, int nodoBase)
    : Unidad(id, std::move(nombre), nodoBase) {}

void Serenazgo::atenderIncidente(const Incidente& inc) {
    // TODO Sprint 1: lógica específica de serenazgo (seguridad)
    m_incidentesAtendidos++;
}

std::string Serenazgo::getTipoUnidad() const { return "Serenazgo"; }
