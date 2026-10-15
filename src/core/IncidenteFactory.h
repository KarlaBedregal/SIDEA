#pragma once
#include <memory>
#include "Incidente.h"

class IncidenteFactory {
public:
    static std::shared_ptr<Incidente> crear(int id, TipoIncidente tipo,
                                             int nodo, PrioridadIncidente prioridad);
    static std::shared_ptr<Incidente> crearAleatorio(int id, int nodoMax);
};
