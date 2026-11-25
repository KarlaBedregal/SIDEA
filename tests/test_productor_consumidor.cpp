#include <gtest/gtest.h>
#include "core/CentralDespacho.h"
#include "core/Ambulancia.h"
#include "core/Bombero.h"
#include "core/Serenazgo.h"
#include "core/Incidente.h"
#include "strategies/EstrategiaDespacho.h"
#include "threading/GeneradorIncidentes.h"
#include <atomic>
#include <chrono>
#include <thread>
#include <stdexcept>

namespace {
using namespace std::chrono_literals;
template<class Predicado> bool esperar(Predicado listo) {
    const auto limite = std::chrono::steady_clock::now() + 3s;
    while (!listo()) {
        if (std::chrono::steady_clock::now() >= limite) return false;
        std::this_thread::sleep_for(1ms);
    }
    return true;
}
void completar(Unidad& unidad) {
    for (int i = 0; i < 100 && !unidad.estaLibre(); ++i) unidad.tick();
    EXPECT_TRUE(unidad.estaLibre());
}
class EstrategiaEnEspera : public EstrategiaDespacho {
    std::atomic<int>& m_llamadas;
public:
    explicit EstrategiaEnEspera(std::atomic<int>& llamadas) : m_llamadas(llamadas) {}
    std::string nombre() const override { return "En espera"; }
    std::shared_ptr<Unidad> seleccionar(const std::vector<std::shared_ptr<Unidad>>&,
                                       const Incidente&) override {
        ++m_llamadas;
        return nullptr;
    }
};
}

TEST(ProductorConsumidorTest, PrioridadReservaEsperaSinSondeoYDiezIncidentesSinPerdidas) {
    auto& central = CentralDespacho::instancia();
    central.reiniciar();
    struct Parada { CentralDespacho& central; ~Parada() { central.reiniciar(); } } parada{central};
    central.setEstrategia(nullptr);
    central.setTickMs(1);
    auto amb = std::make_shared<Ambulancia>(100, "AMB", 0);
    central.registrarUnidad(amb);

    // La prioridad manda y los empates conservan FIFO.
    auto baja = std::make_shared<Incidente>(100, TipoIncidente::URGENCIA_MEDICA, 0, PrioridadIncidente::BAJA);
    auto primera = std::make_shared<Incidente>(101, TipoIncidente::URGENCIA_MEDICA, 0, PrioridadIncidente::CRITICA);
    auto segunda = std::make_shared<Incidente>(102, TipoIncidente::URGENCIA_MEDICA, 0, PrioridadIncidente::CRITICA);
    central.recibirIncidente(baja); central.recibirIncidente(primera); central.recibirIncidente(segunda);
    central.procesarCola();
    EXPECT_EQ(central.totalIncidentesAtendidos(), 0);
    EXPECT_EQ(central.totalIncidentesPendientes(), 2);
    central.procesarCola();
    EXPECT_EQ(central.totalIncidentesPendientes(), 2); // Unidad reservada, sin doble asignación.
    completar(*amb);
    EXPECT_TRUE(primera->estaAtendido());
    EXPECT_FALSE(segunda->estaAtendido()); EXPECT_FALSE(baja->estaAtendido());
    central.procesarCola(); completar(*amb);
    EXPECT_TRUE(segunda->estaAtendido()); EXPECT_FALSE(baja->estaAtendido());
    central.procesarCola(); completar(*amb);
    EXPECT_TRUE(baja->estaAtendido());

    // Sin recurso compatible, conservar el caso hasta registrar la unidad adecuada.
    auto incendio = std::make_shared<Incidente>(103, TipoIncidente::INCENDIO, 3, PrioridadIncidente::CRITICA);
    central.recibirIncidente(incendio);
    central.iniciar();
    std::this_thread::sleep_for(10ms);
    EXPECT_EQ(central.totalIncidentesPendientes(), 1);
    EXPECT_TRUE(amb->estaLibre());
    auto bom = std::make_shared<Bombero>(101, "BOM", 3);
    central.registrarUnidad(bom); // Debe despertar al consumidor e iniciar su hilo.
    ASSERT_TRUE(esperar([&] { return bom->getIncidentesAtendidos() == 1 && bom->estaLibre(); }));
    central.detener();
    EXPECT_TRUE(incendio->estaAtendido());

    // Un recurso no disponible no provoca busy-wait; un cambio de estrategia despierta la cola.
    std::atomic<int> llamadas{0};
    central.setEstrategia(std::make_unique<EstrategiaEnEspera>(llamadas));
    auto esperando = std::make_shared<Incidente>(104, TipoIncidente::URGENCIA_MEDICA, 1, PrioridadIncidente::ALTA);
    central.recibirIncidente(esperando);
    central.iniciar();
    ASSERT_TRUE(esperar([&] { return llamadas.load() > 0; }));
    const int llamadasIniciales = llamadas;
    std::this_thread::sleep_for(20ms);
    EXPECT_EQ(llamadas.load(), llamadasIniciales);
    EXPECT_EQ(central.totalIncidentesPendientes(), 1);
    central.detener();
    EXPECT_EQ(central.totalIncidentesPendientes(), 1);
    central.iniciar();
    ASSERT_TRUE(esperar([&] { return llamadas.load() > llamadasIniciales; }));
    central.setEstrategia(nullptr);
    ASSERT_TRUE(esperar([&] { return amb->getIncidentesAtendidos() == 4 && amb->estaLibre(); }));
    central.detener();
    EXPECT_TRUE(esperando->estaAtendido());

    // Productor limitado a diez casos, consumidor independiente de la GUI.
    auto ser = std::make_shared<Serenazgo>(102, "SER", 4);
    central.registrarUnidad(ser);
    const int atendidosAntes = central.totalIncidentesAtendidos();
    GeneradorIncidentes generador(central, 1, 25, 10); // Recorta al máximo real del mapa.
    central.iniciar();
    generador.iniciar();
    ASSERT_TRUE(esperar([&] {
        return generador.totalGenerados() == 10 && central.totalIncidentesPendientes() == 0 &&
               central.totalIncidentesAtendidos() == atendidosAntes + 10 &&
               amb->estaLibre() && bom->estaLibre() && ser->estaLibre();
    }));
    generador.detener();
    central.detener();
    EXPECT_EQ(generador.totalGenerados(), 10);
    EXPECT_FALSE(generador.estaActivo());
    EXPECT_EQ(central.totalIncidentesAtendidos(), atendidosAntes + 10);
    EXPECT_EQ(central.totalIncidentesPendientes(), 0);

    // Reiniciar y detener la cola vacía también debe terminar sin bloqueos.
    central.iniciar(); central.iniciar();
    const auto inicio = std::chrono::steady_clock::now();
    central.detener(); central.detener();
    EXPECT_LT(std::chrono::steady_clock::now() - inicio, 250ms);
}

TEST(ProductorConsumidorTest, ParadaInterrumpeLaEsperaLargaDelGenerador) {
    GeneradorIncidentes generador(CentralDespacho::instancia(), 5000);
    generador.iniciar(); generador.iniciar();
    const auto inicio = std::chrono::steady_clock::now();
    generador.detener(); generador.detener();
    EXPECT_LT(std::chrono::steady_clock::now() - inicio, 250ms);
    EXPECT_EQ(generador.totalGenerados(), 0);
    generador.iniciar(); generador.detener();
    EXPECT_FALSE(generador.estaActivo());
}

TEST(ProductorConsumidorTest, RechazaParametrosInvalidosSinModificarLaCola) {
    auto& central = CentralDespacho::instancia();
    const int pendientes = central.totalIncidentesPendientes();
    EXPECT_THROW(central.recibirIncidente(nullptr), std::invalid_argument);
    EXPECT_THROW(central.registrarUnidad(nullptr), std::invalid_argument);
    EXPECT_THROW(central.setTickMs(0), std::invalid_argument);
    auto fuera = std::make_shared<Incidente>(200, TipoIncidente::ROBO, 20, PrioridadIncidente::ALTA);
    EXPECT_THROW(central.recibirIncidente(fuera), std::invalid_argument);
    EXPECT_THROW(GeneradorIncidentes(central, 0), std::invalid_argument);
    EXPECT_THROW(GeneradorIncidentes(central, 1, -1), std::invalid_argument);
    EXPECT_EQ(central.totalIncidentesPendientes(), pendientes);
}
