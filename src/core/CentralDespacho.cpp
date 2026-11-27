#include "CentralDespacho.h"
#include "Incidente.h"
#include "Unidad.h"
#include "strategies/EstrategiaDespacho.h"
#include "graph/MapaArequipa.h"
#include "threading/SimuladorThread.h"
#include <algorithm>
#include <stdexcept>
#include <chrono>
#include <ctime>

CentralDespacho::~CentralDespacho() { detener(); }

// Patrón Singleton (Meyers' Singleton):
// La instancia estática local se inicializa de forma lazy y thread-safe (garantizado desde C++11)
// en la primera llamada y se retorna siempre la misma y única referencia en todo el sistema.
CentralDespacho& CentralDespacho::instancia() {
    static CentralDespacho instance;
    return instance;
}

void CentralDespacho::registrarUnidad(std::shared_ptr<Unidad> unidad) {
    if (!unidad) throw std::invalid_argument("La unidad no puede ser nula");
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        for (const auto& registrada : m_unidades) {
            if (registrada == unidad || registrada->getId() == unidad->getId())
                throw std::invalid_argument("La unidad ya está registrada");
        }
        if (m_activa) {
            auto sim = std::make_unique<SimuladorThread>(unidad, *m_mapa, m_tickMs);
            sim->iniciar();
            m_hilos.push_back(std::move(sim));
        }
        m_unidades.push_back(std::move(unidad));
        ++m_revision;
    }
    m_cv.notify_one();
}

void CentralDespacho::recibirIncidente(std::shared_ptr<Incidente> incidente) {
    static const MapaArequipa mapa;
    if (!incidente || !mapa.grafo().existeNodo(incidente->getNodoUbicacion()) ||
        static_cast<int>(incidente->getPrioridad()) < 1 ||
        static_cast<int>(incidente->getPrioridad()) > 4 ||
        static_cast<int>(incidente->getTipo()) < 0 ||
        static_cast<int>(incidente->getTipo()) > 3)
        throw std::invalid_argument("Incidente no válido para el mapa");
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_cola.push_back(std::move(incidente));
        ++m_revision;
    }
    m_cv.notify_one();
    notificar("incidente_recibido");
}

void CentralDespacho::setEstrategia(std::unique_ptr<EstrategiaDespacho> estrategia) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_estrategia = std::move(estrategia);
        ++m_revision;
    }
    m_cv.notify_one();
}

void CentralDespacho::procesarCola() {
    despacharUno();
}

bool CentralDespacho::despacharUno() {
    std::shared_ptr<Unidad> unidadAsignada;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Mantener FIFO para prioridades iguales. Un caso sin recurso compatible
        // espera en la cola sin impedir atender casos que sí tengan recurso.
        auto ordenados = m_cola;
        std::stable_sort(ordenados.begin(), ordenados.end(),
            [](const auto& a, const auto& b) {
                return static_cast<int>(a->getPrioridad()) > static_cast<int>(b->getPrioridad());
            });
        for (const auto& incidente : ordenados) {
            std::vector<std::shared_ptr<Unidad>> disponibles;
            for (const auto& unidad : m_unidades) {
                if (unidad->estaLibre() && unidad->puedeAtender(*incidente))
                    disponibles.push_back(unidad);
            }
            if (disponibles.empty()) continue;
            auto unidad = m_estrategia ? m_estrategia->seleccionar(disponibles, *incidente)
                                      : disponibles.front();
            if (!unidad || std::find(disponibles.begin(), disponibles.end(), unidad) == disponibles.end())
                continue;
            if (!unidad->asignarIncidente(incidente)) continue;
            m_cola.erase(std::find(m_cola.begin(), m_cola.end(), incidente));
            unidadAsignada = std::move(unidad);
            break;
        }
    }
    // Notificar fuera del lock para no bloquear a los observadores
    if (unidadAsignada) notificar("unidad_asignada");
    return static_cast<bool>(unidadAsignada);
}

void CentralDespacho::bucleConsumo() {
    std::unique_lock<std::mutex> lock(m_mutex);
    std::uint64_t revisionProcesada = 0;
    while (m_activa) {
        m_cv.wait(lock, [&] {
            return !m_activa || (!m_cola.empty() && m_revision != revisionProcesada);
        });
        if (!m_activa) break;
        const auto revision = m_revision;
        lock.unlock();
        while (m_activa && despacharUno()) {}
        lock.lock();
        // Si llegó otro evento durante el despacho, se procesará antes de esperar.
        revisionProcesada = revision;
    }
}

void CentralDespacho::iniciar() {
    std::lock_guard<std::mutex> ciclo(m_cicloMutex);
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_activa) return;
    m_activa = true;

    if (!m_mapa) m_mapa = std::make_unique<MapaArequipa>();

    for (const auto& u : m_unidades) {
        auto sim = std::make_unique<SimuladorThread>(u, *m_mapa, m_tickMs);
        sim->iniciar();
        m_hilos.push_back(std::move(sim));
    }
    m_consumidor = std::thread(&CentralDespacho::bucleConsumo, this);
}

void CentralDespacho::detener() {
    std::lock_guard<std::mutex> ciclo(m_cicloMutex);
    detenerHilos();
}

void CentralDespacho::detenerHilos() {
    std::vector<std::unique_ptr<SimuladorThread>> hilos;
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_activa = false;
        hilos.swap(m_hilos);
    }
    m_cv.notify_all();
    if (m_consumidor.joinable()) m_consumidor.join();
    for (auto& h : hilos) {
        if (h) h->detener();
    }
}

void CentralDespacho::setTickMs(int ms) {
    if (ms <= 0) throw std::invalid_argument("El tick debe ser positivo");
    std::lock_guard<std::mutex> lock(m_mutex);
    m_tickMs = ms;
}

// Llamado por EstadoRegresando::manejar() cuando la unidad llega a su nodo base (#7)
void CentralDespacho::notificarUnidadLibre() {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        ++m_revision;
    }
    m_cv.notify_one();
    notificar("unidad_libre");
}

std::vector<std::shared_ptr<Unidad>> CentralDespacho::getUnidades() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_unidades;
}

std::vector<std::shared_ptr<Incidente>> CentralDespacho::getIncidentesPendientes() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cola;
}

void CentralDespacho::registrarIncidenteAtendido(const Incidente& incidente) {
    const auto ahora = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm horaUtc{};
    gmtime_r(&ahora, &horaUtc);
    m_historial.registrar(incidente.getNodoUbicacion(), incidente.getTipo(), horaUtc.tm_hour);
    notificar("incidente_atendido");
}

std::vector<RegistroHistorial> CentralDespacho::getHistorialIncidentes() const {
    return m_historial.todos();
}

std::vector<PrediccionZona> CentralDespacho::zonasDeMayorRiesgo(int cantidad, int horaActual) const {
    static const MapaArequipa mapa;
    const ModeloPrediccion modelo(m_historial, mapa);
    return modelo.topZonas(cantidad, horaActual);
}

int CentralDespacho::totalIncidentesPendientes() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return static_cast<int>(m_cola.size());
}

void CentralDespacho::reiniciar() {
    std::lock_guard<std::mutex> ciclo(m_cicloMutex);
    detenerHilos();
    std::lock_guard<std::mutex> lock(m_mutex);
    m_unidades.clear();
    m_cola.clear();
    m_historial.limpiar();
    m_estrategia.reset();
}

int CentralDespacho::totalIncidentesAtendidos() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    int atendidos = 0;
    for (const auto& unidad : m_unidades) atendidos += unidad->getIncidentesAtendidos();
    return atendidos;
}
