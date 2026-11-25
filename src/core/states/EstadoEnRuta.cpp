#include "EstadoEnRuta.h"
#include "EstadoAtendiendo.h"
#include "core/Unidad.h"
#include "core/Incidente.h"

void EstadoEnRuta::manejar(Unidad& unidad) {
    if (!unidad.m_incidenteActual || !unidad.avanzarEnRuta()) return;
    // Duración simulada: baja=4, media=3, alta=2 y crítica=1 tick de atención.
    unidad.m_ticksAtencionRestantes =
        5 - static_cast<int>(unidad.m_incidenteActual->getPrioridad());
    unidad.cambiarEstado(std::make_unique<EstadoAtendiendo>());
}
