#pragma once
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include <condition_variable>

class CentralDespacho;

class GeneradorIncidentes {
public:
    explicit GeneradorIncidentes(CentralDespacho& central,
                                  int intervaloMs = 2000,
                                  int nodoMax = 19,
                                  int limiteIncidentes = 0);
    ~GeneradorIncidentes();

    void iniciar();
    void pausar();
    void reanudar();
    void detener();
    bool estaActivo() const;
    bool estaPausado() const;

private:
    void bucleGeneracion();

    CentralDespacho& m_central;
    int m_intervaloMs;
    int m_nodoMax;
    int m_limiteIncidentes;
    std::thread m_hilo;
    std::atomic<bool> m_activo{false};
    std::atomic<bool> m_pausado{false};
    mutable std::mutex m_mutex;
    std::condition_variable m_cv;
    int m_contadorId = 0;
};
