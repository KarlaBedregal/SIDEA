#include "EstadoLibre.h"
#include "EstadoEnRuta.h"
#include "core/Unidad.h"
#include "core/Incidente.h"
#include <stdexcept>

void EstadoLibre::manejar(Unidad& unidad) {
    if (!unidad.m_incidenteActual) return;
    if (!unidad.prepararRuta(unidad.m_incidenteActual->getNodoUbicacion()))
        throw std::logic_error("No existe ruta al incidente asignado");
    unidad.cambiarEstado(std::make_unique<EstadoEnRuta>());
}
