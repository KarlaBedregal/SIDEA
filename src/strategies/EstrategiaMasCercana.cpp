#include "EstrategiaMasCercana.h"
#include "core/Unidad.h"
#include "core/Incidente.h"
#include "graph/Dijkstra.h"
#include <limits>

EstrategiaMasCercana::EstrategiaMasCercana(const MapaArequipa& mapa) : m_mapa(mapa) {}

std::shared_ptr<Unidad> EstrategiaMasCercana::seleccionar(
    const std::vector<std::shared_ptr<Unidad>>& unidades,
    const Incidente& incidente)
{
    // TODO Sprint 2: para cada unidad libre, calcular Dijkstra desde su nodo actual
    // al nodo del incidente y devolver la de menor distancia
    std::shared_ptr<Unidad> elegida;
    double menorDist = std::numeric_limits<double>::infinity();

    for (auto& u : unidades) {
        if (!u->estaLibre()) continue;
        auto res = dijkstra(m_mapa.grafo(), u->getNodoActual());
        double dist = res.distanciaA(incidente.getNodoUbicacion());
        if (dist < menorDist) {
            menorDist = dist;
            elegida = u;
        }
    }
    return elegida;
}
