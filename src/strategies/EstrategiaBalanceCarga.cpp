#include "EstrategiaBalanceCarga.h"
#include "core/Unidad.h"
#include "core/Incidente.h"
#include <algorithm>
#include <limits>

std::shared_ptr<Unidad> EstrategiaBalanceCarga::seleccionar(
    const std::vector<std::shared_ptr<Unidad>>& unidades,
    const Incidente& incidente)
{
    // TODO Sprint 2: devolver la unidad libre con menos incidentes atendidos acumulados
    std::shared_ptr<Unidad> elegida;
    int menorCarga = std::numeric_limits<int>::max();

    for (auto& u : unidades) {
        if (u->estaLibre() && u->getIncidentesAtendidos() < menorCarga) {
            menorCarga = u->getIncidentesAtendidos();
            elegida = u;
        }
    }
    return elegida;
}
