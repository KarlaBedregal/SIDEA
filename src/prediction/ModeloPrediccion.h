#pragma once
#include <vector>

class HistorialIncidentes;

struct PrediccionZona {
    int nodo;
    double probabilidad;
};

class ModeloPrediccion {
public:
    explicit ModeloPrediccion(const HistorialIncidentes& historial);

    PrediccionZona zonaMasRiesgosa(int horaActual) const;
    std::vector<PrediccionZona> topZonas(int n, int horaActual) const;
    void actualizar();

private:
    double calcularProbabilidad(int nodo, int hora) const;

    const HistorialIncidentes& m_historial;
};
