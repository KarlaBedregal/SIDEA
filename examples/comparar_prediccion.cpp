// Reproduce los mismos 24 incidentes para las seis combinaciones, sin aleatoriedad.
// Cada iteración representa un tick; no depende de la velocidad de esta PC.
#include "core/CentralDespacho.h"
#include "core/UnidadFactory.h"
#include "core/Incidente.h"
#include "graph/MapaArequipa.h"
#include "strategies/EstrategiaMasCercana.h"
#include "strategies/EstrategiaPrioridad.h"
#include "strategies/EstrategiaBalanceCarga.h"
#include <iomanip>
#include <iostream>

int main() {
    MapaArequipa mapa;
    auto& central = CentralDespacho::instancia();
    std::cout << "estrategia,prediccion,incidentes,llegadas,respuesta_media_ticks,atendidos,fin_tick\n";
    for (int estrategia = 0; estrategia < 3; ++estrategia) {
        for (bool prediccion : {false, true}) {
            central.reiniciar();
            for (int i = 0; i < 8; ++i) {
                const TipoUnidad tipo = i < 3 ? TipoUnidad::AMBULANCIA
                                           : i < 5 ? TipoUnidad::BOMBERO : TipoUnidad::SERENAZGO;
                const int bases[] = {0, 4, 9, 1, 3, 2, 6, 8};
                central.registrarUnidad(UnidadFactory::crear(i + 1, tipo, "U" + std::to_string(i + 1), bases[i]));
            }
            std::string nombre;
            if (estrategia == 0) {
                central.setEstrategia(std::make_unique<EstrategiaMasCercana>(mapa));
                nombre = "Mas cercana";
            } else if (estrategia == 1) {
                central.setEstrategia(std::make_unique<EstrategiaPrioridad>());
                nombre = "Prioridad";
            } else {
                central.setEstrategia(std::make_unique<EstrategiaBalanceCarga>());
                nombre = "Balance de carga";
            }
            central.setReubicacionPreventiva(prediccion);
            struct Caso { std::shared_ptr<Incidente> incidente; int creado; bool llego = false; };
            std::vector<Caso> casos;
            int sumaTicks = 0, llegadas = 0, tick = 0;
            for (; tick < 1000; ++tick) {
                if (tick % 12 == 0 && casos.size() < 24) {
                    const int id = static_cast<int>(casos.size());
                    const int nodo = id % 4 == 0 ? 10 : 3;
                    auto inc = std::make_shared<Incidente>(id, static_cast<TipoIncidente>(id % 4),
                        nodo, static_cast<PrioridadIncidente>(1 + id % 4));
                    casos.push_back({inc, tick});
                    central.recibirIncidente(inc);
                }
                for (int i = 0; i < 8; ++i) central.procesarCola();
                central.reubicarUnidades();
                for (const auto& unidad : central.getUnidades()) unidad->tick();
                for (auto& caso : casos) {
                    if (!caso.llego && caso.incidente->tiempoRespuesta() >= 0) {
                        caso.llego = true;
                        ++llegadas;
                        sumaTicks += tick - caso.creado + 1;
                    }
                }
                bool libres = true;
                for (const auto& unidad : central.getUnidades()) libres &= unidad->estaLibre();
                if (casos.size() == 24 && central.totalIncidentesAtendidos() == 24 && libres) break;
            }
            if (llegadas != 24 || central.totalIncidentesAtendidos() != 24) return 1;
            std::cout << nombre << ',' << (prediccion ? "si" : "no") << ",24," << llegadas << ','
                      << std::fixed << std::setprecision(3) << static_cast<double>(sumaTicks) / llegadas
                      << ',' << central.totalIncidentesAtendidos() << ',' << tick << '\n';
        }
    }
    central.reiniciar();
}
