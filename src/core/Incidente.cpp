#include "Incidente.h"
#include <random>

Incidente::Incidente(int id, TipoIncidente tipo, int nodoUbicacion, PrioridadIncidente prioridad)
    : m_id(id), m_tipo(tipo), m_nodoUbicacion(nodoUbicacion), m_prioridad(prioridad) {}

int Incidente::getId() const { return m_id; }
TipoIncidente Incidente::getTipo() const { return m_tipo; }
int Incidente::getNodoUbicacion() const { return m_nodoUbicacion; }
PrioridadIncidente Incidente::getPrioridad() const { return m_prioridad; }
bool Incidente::estaAtendido() const { return m_atendido; }
void Incidente::marcarAtendido() { m_atendido = true; }

std::string Incidente::tipoToString() const {
    switch (m_tipo) {
        case TipoIncidente::INCENDIO:       return "Incendio";
        case TipoIncidente::ACCIDENTE:      return "Accidente";
        case TipoIncidente::ROBO:           return "Robo";
        case TipoIncidente::URGENCIA_MEDICA: return "Urgencia médica";
    }
    return "Desconocido";
}

std::string Incidente::prioridadToString() const {
    switch (m_prioridad) {
        case PrioridadIncidente::BAJA:   return "Baja";
        case PrioridadIncidente::MEDIA:  return "Media";
        case PrioridadIncidente::ALTA:   return "Alta";
        case PrioridadIncidente::CRITICA: return "Crítica";
    }
    return "Desconocida";
}
