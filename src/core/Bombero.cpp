#include "Bombero.h"
#include "Incidente.h"

Bombero::Bombero(int id, std::string nombre, int nodoBase)
    : Unidad(id, std::move(nombre), nodoBase) {}

void Bombero::atenderIncidente(const Incidente& inc) {
    // TODO Sprint 1: lógica específica de bomberos (extinción)
    m_incidentesAtendidos++;
}

std::string Bombero::getTipoUnidad() const { return "Bombero"; }
