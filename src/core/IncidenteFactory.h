#pragma once
#include <memory>
#include "Incidente.h"

class IncidenteFactory {
public:
    static std::shared_ptr<Incidente> crear(int id, TipoIncidente tipo,
                                             int nodo, PrioridadIncidente prioridad);
    // Genera un incidente sin atender con nodo en [0, nodoMax], ambos incluidos.
    // Lanza std::invalid_argument si nodoMax es negativo.
    static std::shared_ptr<Incidente> crearAleatorio(int id, int nodoMax);
};
