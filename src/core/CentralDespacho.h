#pragma once
#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include <condition_variable>
#include <thread>
#include "patterns/IObservable.h"
#include "prediction/HistorialIncidentes.h"
#include "prediction/ModeloPrediccion.h"

class Incidente;
class Unidad;
class EstrategiaDespacho;
class MapaArequipa;
class SimuladorThread;

struct ResumenComparacion {
    std::string estrategia;
    bool prediccion = false;
    int asignados = 0;
    int atendidos = 0;
    int llegadas = 0;
    double respuestaMediaSegundos = 0.0;
};

class CentralDespacho : public IObservable {
public:
    static CentralDespacho& instancia();
    ~CentralDespacho();

    CentralDespacho(const CentralDespacho&) = delete;
    CentralDespacho& operator=(const CentralDespacho&) = delete;

    void registrarUnidad(std::shared_ptr<Unidad> unidad);
    void recibirIncidente(std::shared_ptr<Incidente> incidente);
    void setEstrategia(std::unique_ptr<EstrategiaDespacho> estrategia);
    void procesarCola();
    void iniciar();
    void detener();
    void setTickMs(int ms);
    void notificarUnidadLibre();
    void registrarIncidenteAtendido(const Incidente& incidente);
    void setReubicacionPreventiva(bool activa);
    void reiniciar();
    void reubicarUnidades();

    std::vector<std::shared_ptr<Unidad>> getUnidades() const;
    std::vector<std::shared_ptr<Incidente>> getIncidentesPendientes() const;
    std::vector<std::shared_ptr<Incidente>> getIncidentesActivos() const;
    std::vector<RegistroHistorial> getHistorialIncidentes() const;
    std::vector<PrediccionZona> zonasDeMayorRiesgo(int cantidad, int horaActual) const;
    std::vector<ResumenComparacion> comparacionPrediccion() const;
    int totalIncidentesPendientes() const;
    int totalIncidentesAtendidos() const;
    bool reubicacionPreventiva() const;

private:
    struct RegistroDespacho {
        std::shared_ptr<Incidente> incidente;
        std::string estrategia;
        bool prediccion = false;
    };

    CentralDespacho() = default;
    bool despacharUno();
    void bucleConsumo();
    void detenerHilos();

    std::vector<std::shared_ptr<Unidad>> m_unidades;
    std::vector<std::shared_ptr<Incidente>> m_cola;
    std::vector<RegistroDespacho> m_registros;
    HistorialIncidentes m_historial;
    std::unique_ptr<EstrategiaDespacho> m_estrategia;
    std::unique_ptr<MapaArequipa> m_mapa;
    std::vector<std::unique_ptr<SimuladorThread>> m_hilos;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_activa{false};
    std::thread m_consumidor;
    std::mutex m_cicloMutex;
    std::uint64_t m_revision = 0;
    int m_tickMs = 500;
    bool m_reubicacionPreventiva = false;
};
