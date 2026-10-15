#include "CentralDespacho.h"
#include "Incidente.h"
#include "Unidad.h"
#include "strategies/EstrategiaDespacho.h"

CentralDespacho& CentralDespacho::instancia() {
    static CentralDespacho instance;
    return instance;
}

void CentralDespacho::registrarUnidad(std::shared_ptr<Unidad> unidad) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_unidades.push_back(std::move(unidad));
}

void CentralDespacho::recibirIncidente(std::shared_ptr<Incidente> incidente) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cola.push_back(std::move(incidente));
    }
    m_cv.notify_one();
}

void CentralDespacho::setEstrategia(std::unique_ptr<EstrategiaDespacho> estrategia) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_estrategia = std::move(estrategia);
}

void CentralDespacho::procesarCola() {
    // TODO Sprint 2: asignar incidentes pendientes usando m_estrategia
}

void CentralDespacho::iniciar() { m_activa = true; }
void CentralDespacho::detener() { m_activa = false; m_cv.notify_all(); }

const std::vector<std::shared_ptr<Unidad>>& CentralDespacho::getUnidades() const {
    return m_unidades;
}

int CentralDespacho::totalIncidentesPendientes() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_cola.size());
}

int CentralDespacho::totalIncidentesAtendidos() const { return m_atendidos; }
