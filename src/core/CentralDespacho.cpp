#include "CentralDespacho.h"
#include "Incidente.h"
#include "Unidad.h"
#include "strategies/EstrategiaDespacho.h"
#include <algorithm>

// Patrón Singleton (Meyers' Singleton):
// La instancia estática local se inicializa de forma lazy y thread-safe (garantizado desde C++11)
// en la primera llamada y se retorna siempre la misma y única referencia en todo el sistema.
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
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_cola.empty()) {
        return;
    }

    // Extraer el incidente con mayor prioridad (CRITICA > ALTA > MEDIA > BAJA)
    // std::max_element conserva el orden de llegada (FIFO) ante prioridades iguales
    auto itMayorPrioridad = std::max_element(
        m_cola.begin(), m_cola.end(),
        [](const std::shared_ptr<Incidente>& a, const std::shared_ptr<Incidente>& b) {
            if (!a) return true;
            if (!b) return false;
            return static_cast<int>(a->getPrioridad()) < static_cast<int>(b->getPrioridad());
        }
    );

    if (itMayorPrioridad == m_cola.end() || !(*itMayorPrioridad)) {
        return;
    }

    auto incidente = *itMayorPrioridad;

    // Buscar una unidad disponible utilizando la estrategia configurada o seleccionando la primera libre
    std::shared_ptr<Unidad> unidad = nullptr;
    if (m_estrategia) {
        unidad = m_estrategia->seleccionar(m_unidades, *incidente);
    } else {
        for (const auto& u : m_unidades) {
            if (u && u->estaLibre()) {
                unidad = u;
                break;
            }
        }
    }

    // Si existe una unidad disponible, se le asigna el incidente y se extrae de la cola
    if (unidad) {
        unidad->asignarIncidente(incidente);
        m_cola.erase(itMayorPrioridad);
        m_atendidos++;
    }
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
