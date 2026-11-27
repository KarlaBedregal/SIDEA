#include "CentralDespacho.h"
#include "Incidente.h"
#include "Unidad.h"
#include "strategies/EstrategiaDespacho.h"
#include "graph/MapaArequipa.h"
#include "threading/SimuladorThread.h"
#include "prediction/ModeloPrediccion.h"
#include <algorithm>
#include <chrono>
#include <iterator>
#include <stdexcept>

namespace {
int horaActual() {
    return static_cast<int>(std::chrono::duration_cast<std::chrono::hours>(
        std::chrono::system_clock::now().time_since_epoch()).count() % 24);
}
}

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
        if (incidente->estaAtendido() || std::any_of(m_registros.begin(), m_registros.end(),
            [&](const auto& r) { return r.incidente->getId() == incidente->getId(); }))
            throw std::invalid_argument("Incidente atendido o identificador duplicado");
        m_historial.registrar(incidente->getNodoUbicacion(), incidente->getTipo(), horaActual());
        m_registros.push_back({incidente, {}, false});
        m_cola.push_back(std::move(incidente));
        ++m_revision;
    }
    m_cv.notify_one();
    notificar("incidente_recibido");
    reubicarUnidades();
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
            for (auto& registro : m_registros) {
                if (registro.incidente == incidente) {
                    registro.estrategia = m_estrategia ? m_estrategia->nombre() : "Sin estrategia";
                    registro.prediccion = m_reubicacionPreventiva;
                    break;
                }
            }
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
    reubicarUnidades();
}

std::vector<std::shared_ptr<Incidente>> CentralDespacho::getIncidentesActivos() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<std::shared_ptr<Incidente>> resultado;
    for (const auto& r : m_registros)
        if (!r.incidente->estaAtendido()) resultado.push_back(r.incidente);
    return resultado;
}

void CentralDespacho::setReubicacionPreventiva(bool activa) {
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_reubicacionPreventiva = activa;
        if (!activa)
            for (const auto& unidad : m_unidades) unidad->cancelarReubicacion();
    }
    if (activa) reubicarUnidades();
    notificar("prediccion_actualizada");
}

bool CentralDespacho::reubicacionPreventiva() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_reubicacionPreventiva;
}

void CentralDespacho::reubicarUnidades() {
    static const MapaArequipa mapa;
    std::lock_guard<std::mutex> lock(m_mutex);
    if (!m_reubicacionPreventiva) return;
    const auto zona = ModeloPrediccion(m_historial, mapa).zonaMasRiesgosa(horaActual());
    if (zona.nodo < 0 || zona.probabilidad <= 0.0) return;
    for (const auto& unidad : m_unidades) {
        if (!unidad->estaLibre()) continue;
        // El despacho compatible siempre tiene precedencia sobre la prevención.
        const bool pendiente = std::any_of(m_cola.begin(), m_cola.end(),
            [&](const auto& inc) { return unidad->puedeAtender(*inc); });
        if (!pendiente) unidad->reubicar(zona.nodo);
    }
}

std::vector<ResumenComparacion> CentralDespacho::comparacionPrediccion() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    std::vector<ResumenComparacion> resultado;
    for (const auto& registro : m_registros) {
        if (registro.estrategia.empty()) continue;
        auto it = std::find_if(resultado.begin(), resultado.end(), [&](const auto& resumen) {
            return resumen.estrategia == registro.estrategia && resumen.prediccion == registro.prediccion;
        });
        if (it == resultado.end()) {
            resultado.push_back({registro.estrategia, registro.prediccion});
            it = std::prev(resultado.end());
        }
        ++it->asignados;
        if (registro.incidente->estaAtendido()) ++it->atendidos;
        const double respuesta = registro.incidente->tiempoRespuesta();
        if (respuesta >= 0.0) {
            it->respuestaMediaSegundos += respuesta;
            ++it->llegadas;
        }
    }
    for (auto& resumen : resultado)
        if (resumen.llegadas) resumen.respuestaMediaSegundos /= resumen.llegadas;
    return resultado;
}

std::vector<std::shared_ptr<Unidad>> CentralDespacho::getUnidades() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_unidades;
}

std::vector<std::shared_ptr<Incidente>> CentralDespacho::getIncidentesPendientes() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_cola;
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
    m_estrategia.reset();
    m_historial = HistorialIncidentes{};
    m_registros.clear();
    m_reubicacionPreventiva = false;
    ++m_revision;
}

int CentralDespacho::totalIncidentesAtendidos() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    int atendidos = 0;
    for (const auto& unidad : m_unidades) atendidos += unidad->getIncidentesAtendidos();
    return atendidos;
}
