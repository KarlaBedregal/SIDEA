#pragma once
#include <string>
#include <atomic>
#include <chrono>

enum class TipoIncidente { INCENDIO, ACCIDENTE, ROBO, URGENCIA_MEDICA };
enum class PrioridadIncidente { BAJA = 1, MEDIA = 2, ALTA = 3, CRITICA = 4 };

class Incidente {
public:
    Incidente(int id, TipoIncidente tipo, int nodoUbicacion, PrioridadIncidente prioridad);

    int getId() const;
    TipoIncidente getTipo() const;
    int getNodoUbicacion() const;
    PrioridadIncidente getPrioridad() const;
    bool estaAtendido() const;
    void marcarAtendido();
    void marcarLlegada();
    double tiempoRespuesta() const;
    std::string tipoToString() const;
    std::string prioridadToString() const;

private:
    int m_id;
    TipoIncidente m_tipo;
    int m_nodoUbicacion;
    PrioridadIncidente m_prioridad;
    std::atomic<bool> m_atendido{false};
    std::chrono::steady_clock::time_point m_creado = std::chrono::steady_clock::now();
    std::atomic<double> m_tiempoRespuesta{-1.0};
};
