#include "Estadisticas.h"
#include <numeric>

void Estadisticas::registrarTiempoRespuesta(double segundos) {
    m_tiemposRespuesta.push_back(segundos);
}

void Estadisticas::registrarIncidenteAtendido() { m_atendidos++; }
void Estadisticas::registrarIncidentePendiente() { m_pendientes++; }

void Estadisticas::agregarDatosUnidad(EstadisticaUnidad datos) {
    m_unidades.push_back(std::move(datos));
}

void Estadisticas::reset() {
    m_tiemposRespuesta.clear();
    m_unidades.clear();
    m_atendidos = 0;
    m_pendientes = 0;
}

double Estadisticas::tiempoPromedioRespuesta() const {
    if (m_tiemposRespuesta.empty()) return 0.0;
    double suma = std::accumulate(m_tiemposRespuesta.begin(), m_tiemposRespuesta.end(), 0.0);
    return suma / static_cast<double>(m_tiemposRespuesta.size());
}

int Estadisticas::totalAtendidos() const { return m_atendidos; }
int Estadisticas::totalPendientes() const { return m_pendientes; }

const std::vector<EstadisticaUnidad>& Estadisticas::datosUnidades() const {
    return m_unidades;
}
