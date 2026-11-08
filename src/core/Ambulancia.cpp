#include "Ambulancia.h"
#include "Incidente.h"

Ambulancia::Ambulancia(int id, std::string nombre, int nodoBase)
    : Unidad(id, std::move(nombre), nodoBase) {}

void Ambulancia::atenderIncidente(const Incidente& inc) {
    if (inc.getTipo() == TipoIncidente::URGENCIA_MEDICA) {
        ++m_incidentesAtendidos;
    }
}

std::string Ambulancia::getTipoUnidad() const { return "Ambulancia"; }
