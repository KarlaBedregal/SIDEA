#include "ModeloPrediccion.h"
#include "HistorialIncidentes.h"
#include <algorithm>

ModeloPrediccion::ModeloPrediccion(const HistorialIncidentes& historial)
    : m_historial(historial) {}

double ModeloPrediccion::calcularProbabilidad(int nodo, int hora) const {
    int total = static_cast<int>(m_historial.todos().size());
    if (total == 0) return 0.0;
    double freqZona = m_historial.frecuenciaPorZona(nodo);
    double freqHora = m_historial.frecuenciaPorHora(nodo, hora);
    // modelo simple: combina frecuencia global por zona y por hora
    return (0.6 * freqZona / total) + (0.4 * freqHora / std::max(total, 1));
}

PrediccionZona ModeloPrediccion::zonaMasRiesgosa(int horaActual) const {
    auto top = topZonas(1, horaActual);
    if (top.empty()) return {-1, 0.0};
    return top[0];
}

std::vector<PrediccionZona> ModeloPrediccion::topZonas(int n, int horaActual) const {
    // TODO Sprint 3: recorrer todos los nodos del mapa y calcular probabilidad
    std::vector<PrediccionZona> resultado;
    // Placeholder hasta tener acceso al mapa
    return resultado;
}

void ModeloPrediccion::actualizar() {
    // TODO Sprint 3: re-calcular modelo con datos nuevos del historial
}
