#pragma once
#include <vector>

class HistorialIncidentes;
class MapaArequipa;

struct PrediccionZona {
    int nodo;
    double probabilidad;
};

class ModeloPrediccion {
public:
    ModeloPrediccion(const HistorialIncidentes& historial, const MapaArequipa& mapa);

    PrediccionZona zonaMasRiesgosa(int horaActual) const;
    std::vector<PrediccionZona> zonasDeMayorRiesgo(int horaActual) const;
    std::vector<PrediccionZona> topZonas(int n, int horaActual) const;
    void actualizar();

private:
    double calcularProbabilidad(int nodo, int hora) const;

    const HistorialIncidentes& m_historial;
    const MapaArequipa& m_mapa;
};
