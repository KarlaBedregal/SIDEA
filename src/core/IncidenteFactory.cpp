#include "IncidenteFactory.h"
#include <random>
#include <stdexcept>

std::shared_ptr<Incidente> IncidenteFactory::crear(int id, TipoIncidente tipo,
                                                     int nodo, PrioridadIncidente prioridad) {
    return std::make_shared<Incidente>(id, tipo, nodo, prioridad);
}

std::shared_ptr<Incidente> IncidenteFactory::crearAleatorio(int id, int nodoMax) {
    if (nodoMax < 0) {
        throw std::invalid_argument("El nodo maximo no puede ser negativo");
    }

    // Cada hilo mantiene su propio generador para evitar compartir estado mutable.
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> distTipo(
        static_cast<int>(TipoIncidente::INCENDIO),
        static_cast<int>(TipoIncidente::URGENCIA_MEDICA));
    std::uniform_int_distribution<int> distNodo(0, nodoMax);
    std::uniform_int_distribution<int> distPrio(
        static_cast<int>(PrioridadIncidente::BAJA),
        static_cast<int>(PrioridadIncidente::CRITICA));

    auto tipo = static_cast<TipoIncidente>(distTipo(rng));
    auto prio = static_cast<PrioridadIncidente>(distPrio(rng));
    return crear(id, tipo, distNodo(rng), prio);
}
