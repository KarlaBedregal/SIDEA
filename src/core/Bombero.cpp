#include "Bombero.h"
#include "Incidente.h"

Bombero::Bombero(int id, std::string nombre, int nodoBase)
    : Unidad(id, std::move(nombre), nodoBase) {}

void Bombero::atenderIncidente(const Incidente& inc) {
    const auto tipo = inc.getTipo();
    if (tipo == TipoIncidente::INCENDIO || tipo == TipoIncidente::ACCIDENTE) {
        ++m_incidentesAtendidos;
    }
}

std::string Bombero::getTipoUnidad() const { return "Bombero"; }
