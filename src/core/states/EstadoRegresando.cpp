#include "EstadoRegresando.h"
#include "EstadoLibre.h"
#include "core/Unidad.h"

void EstadoRegresando::manejar(Unidad& unidad) {
    if (!unidad.avanzarEnRuta()) return;
    unidad.m_tiempoOcupado += std::chrono::duration<double>(
        std::chrono::steady_clock::now() - unidad.m_inicioOcupacion).count();
    unidad.m_incidenteActual.reset();
    unidad.m_ruta.clear();
    unidad.m_indiceRuta = 0;
    unidad.cambiarEstado(std::make_unique<EstadoLibre>());
}
