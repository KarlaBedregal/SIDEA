#include "core/Ambulancia.h"
#include "core/Incidente.h"
#include <iostream>

int main() {
    Ambulancia unidad(1, "AMB-01", 0);
    auto incidente = std::make_shared<Incidente>(
        1, TipoIncidente::URGENCIA_MEDICA, 3, PrioridadIncidente::ALTA);
    std::cout << unidad.getEstadoNombre() << '\n';
    if (!unidad.asignarIncidente(incidente)) return 1;
    for (int tick = 0; tick < 100 && !unidad.estaLibre(); ++tick) {
        unidad.tick();
        std::cout << unidad.getEstadoNombre() << " (nodo "
                  << unidad.getNodoActual() << ")\n";
    }
    return unidad.estaLibre() && incidente->estaAtendido() &&
           unidad.getNodoActual() == unidad.getNodoBase() ? 0 : 1;
}
