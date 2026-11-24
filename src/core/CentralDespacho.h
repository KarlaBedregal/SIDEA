#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <condition_variable>
#include "patterns/IObservable.h"

class Incidente;
class Unidad;
class EstrategiaDespacho;
class MapaArequipa;
class SimuladorThread;

class CentralDespacho : public IObservable {
public:
    static CentralDespacho& instancia();

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

    const std::vector<std::shared_ptr<Unidad>>& getUnidades() const;
    int totalIncidentesPendientes() const;
    int totalIncidentesAtendidos() const;
    void reiniciar();

private:
    CentralDespacho() = default;

    std::vector<std::shared_ptr<Unidad>> m_unidades;
    std::vector<std::shared_ptr<Incidente>> m_cola;
    std::unique_ptr<EstrategiaDespacho> m_estrategia;
    std::unique_ptr<MapaArequipa> m_mapa;
    std::vector<std::unique_ptr<SimuladorThread>> m_hilos;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_activa = false;
    int m_atendidos = 0;
    int m_tickMs = 500;
};
