#include "Ambulancia.h"
#include "Incidente.h"
#include <iostream>

Ambulancia::Ambulancia(int id, std::string nombre, int nodoBase)
    : Unidad(id, std::move(nombre), nodoBase) {}

void Ambulancia::atenderIncidente(const Incidente& inc) {
    // TODO Sprint 1: lógica específica de ambulancia (atención médica)
    m_incidentesAtendidos++;
}

std::string Ambulancia::getTipoUnidad() const { return "Ambulancia"; }
