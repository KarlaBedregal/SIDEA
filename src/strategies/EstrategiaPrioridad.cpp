#include "EstrategiaPrioridad.h"
#include "core/Unidad.h"
#include "core/Incidente.h"

std::shared_ptr<Unidad> EstrategiaPrioridad::seleccionar(
    const std::vector<std::shared_ptr<Unidad>>& unidades,
    const Incidente& incidente)
{
    // TODO Sprint 2: devolver la primera unidad libre que coincida con el tipo de incidente
    // Si ninguna coincide, devolver cualquier unidad libre
    for (auto& u : unidades) {
        if (u->estaLibre()) return u;
    }
    return nullptr;
}
