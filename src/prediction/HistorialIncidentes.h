#pragma once
#include <chrono>
#include <cstddef>
#include <mutex>
#include <string>
#include <vector>
#include <unordered_map>
#include "core/Incidente.h"

struct RegistroHistorial {
    int nodo;
    TipoIncidente tipo;
    int hora;
    std::chrono::system_clock::time_point timestamp;
};

class HistorialIncidentes {
public:
    explicit HistorialIncidentes(std::size_t capacidad = 1000);

    void registrar(int nodo, TipoIncidente tipo, int horaSimulacion);

    int frecuenciaPorZona(int nodo) const;
    int frecuenciaPorHora(int nodo, int hora) const;
    std::vector<RegistroHistorial> todos() const;
    std::size_t capacidad() const;
    void limpiar();
    void exportarCSV(const std::string& ruta) const;

private:
    const std::size_t m_capacidad;
    mutable std::mutex m_mutex;
    std::vector<RegistroHistorial> m_registros;
    std::unordered_map<int, int> m_frecuenciaNodo;
};
