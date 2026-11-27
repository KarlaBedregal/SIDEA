#include "EstadoLibre.h"
#include "EstadoEnRuta.h"
#include "core/Unidad.h"
#include "core/Incidente.h"
#include <stdexcept>

void EstadoLibre::manejar(Unidad& unidad) {
    if (!unidad.m_incidenteActual) {
        if (unidad.m_reubicando && unidad.avanzarEnRuta()) unidad.cancelarReubicacion();
        return;
    }
    if (!unidad.prepararRuta(unidad.m_incidenteActual->getNodoUbicacion()))
        throw std::logic_error("No existe ruta al incidente asignado");
    unidad.cambiarEstado(std::make_unique<EstadoEnRuta>());
}
