#include "ModeloPrediccion.h"
#include "HistorialIncidentes.h"
#include "graph/MapaArequipa.h"
#include <algorithm>

ModeloPrediccion::ModeloPrediccion(const HistorialIncidentes& historial, const MapaArequipa& mapa)
    : m_historial(historial), m_mapa(mapa) {}

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
    std::vector<PrediccionZona> resultado;
    for (int nodo : m_mapa.grafo().nodos()) {
        resultado.push_back({nodo, calcularProbabilidad(nodo, horaActual)});
    }
    std::sort(resultado.begin(), resultado.end(),
              [](const PrediccionZona& a, const PrediccionZona& b) {
                  return a.probabilidad > b.probabilidad;
              });
    if (n > 0 && static_cast<int>(resultado.size()) > n)
        resultado.resize(n);
    return resultado;
}

void ModeloPrediccion::actualizar() {
    // El modelo recalcula dinámicamente en cada llamada a topZonas().
}
