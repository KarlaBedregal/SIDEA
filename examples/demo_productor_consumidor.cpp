#include "core/Ambulancia.h"
#include "core/Bombero.h"
#include "core/Serenazgo.h"
#include "core/CentralDespacho.h"
#include "strategies/EstrategiaMasCercana.h"
#include "threading/GeneradorIncidentes.h"
#include "graph/MapaArequipa.h"
#include <chrono>
#include <iostream>
#include <thread>

int main() {
    auto& central = CentralDespacho::instancia();
    central.registrarUnidad(std::make_shared<Ambulancia>(1, "AMB", 0));
    central.registrarUnidad(std::make_shared<Bombero>(2, "BOM", 3));
    central.registrarUnidad(std::make_shared<Serenazgo>(3, "SER", 4));
    MapaArequipa mapa;
    central.setEstrategia(std::make_unique<EstrategiaMasCercana>(mapa));
    central.setTickMs(5);
    GeneradorIncidentes generador(central, 10, mapa.totalNodos() - 1, 10);
    central.iniciar();
    generador.iniciar();
    const auto limite = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    bool completo = false;
    while (std::chrono::steady_clock::now() < limite) {
        completo = generador.totalGenerados() == 10 &&
                   central.totalIncidentesAtendidos() == 10;
        for (const auto& unidad : central.getUnidades()) completo &= unidad->estaLibre();
        if (completo) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    generador.detener();
    central.detener();
    std::cout << "Generados: " << generador.totalGenerados()
              << " | Atendidos: " << central.totalIncidentesAtendidos()
              << " | Pendientes: " << central.totalIncidentesPendientes() << '\n';
    return completo && central.totalIncidentesPendientes() == 0 ? 0 : 1;
}
