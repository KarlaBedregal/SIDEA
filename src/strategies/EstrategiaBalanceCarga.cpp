#include "EstrategiaBalanceCarga.h"
#include "core/Unidad.h"

std::shared_ptr<Unidad> EstrategiaBalanceCarga::seleccionar(
    const std::vector<std::shared_ptr<Unidad>>& unidades,
    const Incidente&)
{
    std::shared_ptr<Unidad> elegida;
    int menorCarga = 0;

    for (const auto& unidad : unidades) {
        if (!unidad || !unidad->estaLibre()) continue;
        const int carga = unidad->getIncidentesAtendidos();
        if (!elegida || carga < menorCarga) {
            menorCarga = carga;
            elegida = unidad;
        }
    }
    return elegida;
}
