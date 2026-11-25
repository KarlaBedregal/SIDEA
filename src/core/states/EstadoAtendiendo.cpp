#include "EstadoAtendiendo.h"
#include "EstadoRegresando.h"
#include "core/Unidad.h"
#include "core/Incidente.h"
#include <stdexcept>

void EstadoAtendiendo::manejar(Unidad& unidad) {
    if (!unidad.m_incidenteActual) return;
    if (--unidad.m_ticksAtencionRestantes > 0) return;
    if (!unidad.prepararRuta(unidad.m_nodoBase))
        throw std::logic_error("No existe ruta de regreso a la base");
    unidad.atenderIncidente(*unidad.m_incidenteActual);
    unidad.m_incidenteActual->marcarAtendido();
    unidad.cambiarEstado(std::make_unique<EstadoRegresando>());
}
