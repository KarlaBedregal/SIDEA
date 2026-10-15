#pragma once
#include <thread>
#include <atomic>
#include <chrono>

class CentralDespacho;

class GeneradorIncidentes {
public:
    explicit GeneradorIncidentes(CentralDespacho& central,
                                  int intervaloMs = 2000,
                                  int nodoMax = 25);
    ~GeneradorIncidentes();

    void iniciar();
    void detener();
    bool estaActivo() const;

private:
    void bucleGeneracion();

    CentralDespacho& m_central;
    int m_intervaloMs;
    int m_nodoMax;
    std::thread m_hilo;
    std::atomic<bool> m_activo{false};
    int m_contadorId = 0;
};
