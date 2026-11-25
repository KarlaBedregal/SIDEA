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
    std::shared_ptr<Unidad> elegida = nullptr;
    double menorDist = std::numeric_limits<double>::infinity();
    const int destino = incidente.getNodoUbicacion();

    for (const auto& u : unidades) {
        if (!u || !u->estaLibre()) continue;

        auto res = dijkstra(m_mapa.grafo(), u->getNodoActual());
        if (!res.alcanzable(destino)) continue;

        double dist = res.distanciaA(destino);
        if (dist < menorDist) {
            menorDist = dist;
            elegida = u;
        }
    }
    return elegida;
}
