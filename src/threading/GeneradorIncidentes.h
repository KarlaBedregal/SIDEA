#pragma once
#include <thread>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>

class CentralDespacho;

class GeneradorIncidentes {
public:
    explicit GeneradorIncidentes(CentralDespacho& central,
                                  int intervaloMs = 2000,
                                  int nodoMax = 19,
                                  int limiteIncidentes = 0);
    ~GeneradorIncidentes();

    void iniciar();
    void detener();
    bool estaActivo() const;
    int totalGenerados() const;

private:
    void bucleGeneracion();

    CentralDespacho& m_central;
    int m_intervaloMs;
    int m_nodoMax;
    int m_limiteIncidentes;
    std::thread m_hilo;
    std::atomic<bool> m_activo{false};
    std::atomic<int> m_generados{0};
    std::mutex m_cicloMutex;
    std::mutex m_mutexEspera;
    std::condition_variable m_cvParada;
    int m_contadorId = 0;
};
