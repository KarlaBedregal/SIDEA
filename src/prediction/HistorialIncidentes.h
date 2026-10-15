#pragma once
#include <vector>
#include <unordered_map>
#include "core/Incidente.h"

struct RegistroHistorial {
    int nodo;
    TipoIncidente tipo;
    int hora;
};

class HistorialIncidentes {
public:
    void registrar(int nodo, TipoIncidente tipo, int horaSimulacion);

    int frecuenciaPorZona(int nodo) const;
    int frecuenciaPorHora(int nodo, int hora) const;
    std::vector<RegistroHistorial> todos() const;
    void exportarCSV(const std::string& ruta) const;

private:
    std::vector<RegistroHistorial> m_registros;
    std::unordered_map<int, int> m_frecuenciaNodo;
};
