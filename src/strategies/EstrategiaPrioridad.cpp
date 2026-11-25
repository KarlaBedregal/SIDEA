#include "EstrategiaPrioridad.h"
#include "core/Unidad.h"
#include "core/Incidente.h"

std::shared_ptr<Unidad> EstrategiaPrioridad::seleccionar(
    const std::vector<std::shared_ptr<Unidad>>& unidades,
    const Incidente& incidente)
{
    const char* tipoPreferido = "";
    switch (incidente.getTipo()) {
        case TipoIncidente::URGENCIA_MEDICA:
            tipoPreferido = "Ambulancia";
            break;
        case TipoIncidente::INCENDIO:
        case TipoIncidente::ACCIDENTE:
            tipoPreferido = "Bombero";
            break;
        case TipoIncidente::ROBO:
            tipoPreferido = "Serenazgo";
            break;
    }

    std::shared_ptr<Unidad> primeraLibre;
    for (const auto& unidad : unidades) {
        if (!unidad || !unidad->estaLibre()) continue;
        if (unidad->getTipoUnidad() == tipoPreferido) return unidad;
        if (!primeraLibre) primeraLibre = unidad;
    }
    return primeraLibre;
}
