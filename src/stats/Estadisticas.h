#pragma once
#include <vector>
#include <string>
#include <unordered_map>

struct EstadisticaUnidad {
    std::string nombre;
    std::string tipo;
    int incidentesAtendidos;
    double porcentajeUso;
};

class Estadisticas {
public:
    void registrarTiempoRespuesta(double segundos);
    void registrarIncidenteAtendido();
    void registrarIncidentePendiente();
    void agregarDatosUnidad(EstadisticaUnidad datos);
    void reset();

    double tiempoPromedioRespuesta() const;
    int totalAtendidos() const;
    int totalPendientes() const;
    const std::vector<EstadisticaUnidad>& datosUnidades() const;

private:
    std::vector<double> m_tiemposRespuesta;
    int m_atendidos = 0;
    int m_pendientes = 0;
    std::vector<EstadisticaUnidad> m_unidades;
};
