#include "IncidenteFactory.h"
#include <random>

std::shared_ptr<Incidente> IncidenteFactory::crear(int id, TipoIncidente tipo,
                                                     int nodo, PrioridadIncidente prioridad) {
    return std::make_shared<Incidente>(id, tipo, nodo, prioridad);
}

std::shared_ptr<Incidente> IncidenteFactory::crearAleatorio(int id, int nodoMax) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> distTipo(0, 3);
    std::uniform_int_distribution<int> distNodo(0, nodoMax);
    std::uniform_int_distribution<int> distPrio(1, 4);

    auto tipo = static_cast<TipoIncidente>(distTipo(rng));
    auto prio = static_cast<PrioridadIncidente>(distPrio(rng));
    return std::make_shared<Incidente>(id, tipo, distNodo(rng), prio);
}
