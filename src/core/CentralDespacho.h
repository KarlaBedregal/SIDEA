#pragma once
#include <memory>
#include <mutex>
#include <vector>
#include <condition_variable>

class Incidente;
class Unidad;
class EstrategiaDespacho;

class CentralDespacho {
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

    const std::vector<std::shared_ptr<Unidad>>& getUnidades() const;
    int totalIncidentesPendientes() const;
    int totalIncidentesAtendidos() const;

private:
    CentralDespacho() = default;

    std::vector<std::shared_ptr<Unidad>> m_unidades;
    std::vector<std::shared_ptr<Incidente>> m_cola;
    std::unique_ptr<EstrategiaDespacho> m_estrategia;
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    bool m_activa = false;
    int m_atendidos = 0;
};
