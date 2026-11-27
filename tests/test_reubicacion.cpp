#include <gtest/gtest.h>
#include "core/CentralDespacho.h"
#include "core/Ambulancia.h"
#include "core/Incidente.h"
#include "graph/MapaArequipa.h"
#include "prediction/HistorialIncidentes.h"
#include "prediction/ModeloPrediccion.h"
#include "strategies/EstrategiaMasCercana.h"
#include <chrono>
#include <thread>

namespace {
auto incidente(int id, int nodo, TipoIncidente tipo = TipoIncidente::URGENCIA_MEDICA) {
    return std::make_shared<Incidente>(id, tipo, nodo, PrioridadIncidente::CRITICA);
}
class ReubicacionTest : public ::testing::Test {
protected:
    CentralDespacho& central = CentralDespacho::instancia();
    void SetUp() override { central.reiniciar(); }
    void TearDown() override { central.reiniciar(); }
};
}

TEST(PrediccionTest, SinDatosEmpatesLimitesYNodosInvalidos) {
    MapaArequipa mapa;
    HistorialIncidentes historial;
    ModeloPrediccion modelo(historial, mapa);
    EXPECT_EQ(modelo.zonaMasRiesgosa(10).nodo, -1);
    historial.registrar(3, TipoIncidente::ROBO, 10);
    historial.registrar(2, TipoIncidente::ROBO, 10);
    historial.registrar(99, TipoIncidente::ROBO, 10);
    EXPECT_EQ(modelo.zonaMasRiesgosa(10).nodo, 2);
    EXPECT_EQ(modelo.topZonas(20, 10).size(), 2u);
    EXPECT_TRUE(modelo.topZonas(0, 10).empty());
    EXPECT_TRUE(modelo.topZonas(-1, 10).empty());
    historial.registrar(3, TipoIncidente::ROBO, 10);
    EXPECT_EQ(modelo.zonaMasRiesgosa(10).nodo, 3);
}

TEST_F(ReubicacionTest, RecorreAristasSinTeletransporteYConservaBase) {
    auto unidad = std::make_shared<Ambulancia>(1, "AMB", 0);
    central.registrarUnidad(unidad);
    // Una emergencia incompatible aporta historia sin reservar la ambulancia.
    central.recibirIncidente(incidente(10, 3, TipoIncidente::ROBO));
    central.setReubicacionPreventiva(true);
    EXPECT_EQ(unidad->getNodoActual(), 0);
    EXPECT_TRUE(unidad->estaLibre());
    unidad->tick(); EXPECT_EQ(unidad->getNodoActual(), 4);
    unidad->tick(); EXPECT_EQ(unidad->getNodoActual(), 2);
    unidad->tick(); EXPECT_EQ(unidad->getNodoActual(), 3);
    EXPECT_EQ(unidad->getNodoBase(), 0);
    EXPECT_EQ(unidad->getIncidentesAtendidos(), 0);
    EXPECT_DOUBLE_EQ(unidad->getTiempoOcupado(), 0.0);
}

TEST_F(ReubicacionTest, DesactivarDetieneRecorridoYDespachoLoInterrumpe) {
    auto unidad = std::make_shared<Ambulancia>(1, "AMB", 0);
    central.registrarUnidad(unidad);
    central.recibirIncidente(incidente(10, 3, TipoIncidente::ROBO));
    central.setReubicacionPreventiva(true);
    unidad->tick();
    central.setReubicacionPreventiva(false);
    unidad->tick(); EXPECT_EQ(unidad->getNodoActual(), 4);
    central.setReubicacionPreventiva(true);
    auto emergencia = incidente(11, 0);
    central.recibirIncidente(emergencia);
    central.procesarCola();
    EXPECT_FALSE(unidad->estaLibre());
    for (int i = 0; i < 30 && !emergencia->estaAtendido(); ++i) unidad->tick();
    EXPECT_TRUE(emergencia->estaAtendido());
    EXPECT_GE(emergencia->tiempoRespuesta(), 0.0);
    EXPECT_EQ(unidad->getIncidentesAtendidos(), 1);
}

TEST_F(ReubicacionTest, SinHistoriaNoMueveYLaColaCompatibleTienePrecedencia) {
    auto unidad = std::make_shared<Ambulancia>(1, "AMB", 0);
    central.registrarUnidad(unidad);
    central.setReubicacionPreventiva(true);
    unidad->tick(); EXPECT_EQ(unidad->getNodoActual(), 0);
    central.recibirIncidente(incidente(1, 3));
    central.reubicarUnidades();
    unidad->tick(); EXPECT_EQ(unidad->getNodoActual(), 0);
    central.procesarCola();
    EXPECT_FALSE(unidad->reubicar(2));
}

TEST_F(ReubicacionTest, ReubicaAlLiberarseYRegistraComparacionPorModoDeAsignacion) {
    MapaArequipa mapa;
    central.setEstrategia(std::make_unique<EstrategiaMasCercana>(mapa));
    auto unidad = std::make_shared<Ambulancia>(1, "AMB", 0);
    central.registrarUnidad(unidad);
    auto primero = incidente(1, 3);
    central.recibirIncidente(primero);
    EXPECT_EQ(central.getIncidentesPendientes().size(), 1u);
    central.procesarCola();
    EXPECT_TRUE(central.getIncidentesPendientes().empty());
    EXPECT_EQ(central.getIncidentesActivos().size(), 1u);
    // Cambiar el toggle con un incidente en curso conserva su cohorte original.
    central.setReubicacionPreventiva(true);
    for (int i = 0; i < 30 && !unidad->estaLibre(); ++i) unidad->tick();
    EXPECT_TRUE(unidad->estaLibre());
    EXPECT_TRUE(central.getIncidentesActivos().empty());
    unidad->tick(); EXPECT_EQ(unidad->getNodoActual(), 4);
    auto segundo = incidente(2, 3);
    central.recibirIncidente(segundo);
    central.procesarCola();
    for (int i = 0; i < 30 && !segundo->estaAtendido(); ++i) unidad->tick();
    auto resumen = central.comparacionPrediccion();
    ASSERT_EQ(resumen.size(), 2u);
    EXPECT_FALSE(resumen[0].prediccion);
    EXPECT_TRUE(resumen[1].prediccion);
    for (const auto& fila : resumen) {
        EXPECT_EQ(fila.asignados, 1);
        EXPECT_EQ(fila.llegadas, 1);
        EXPECT_EQ(fila.atendidos, 1);
        EXPECT_GE(fila.respuestaMediaSegundos, 0.0);
    }
    central.reiniciar();
    EXPECT_FALSE(central.reubicacionPreventiva());
    EXPECT_TRUE(central.comparacionPrediccion().empty());
    EXPECT_TRUE(central.getIncidentesActivos().empty());
}

TEST_F(ReubicacionTest, RechazaDuplicadosYDestinoInvalido) {
    Ambulancia unidad(1, "AMB", 0);
    EXPECT_FALSE(unidad.reubicar(-1));
    EXPECT_FALSE(unidad.reubicar(20));
    auto inc = incidente(1, 3);
    central.recibirIncidente(inc);
    EXPECT_THROW(central.recibirIncidente(inc), std::invalid_argument);
    EXPECT_EQ(central.getIncidentesPendientes().size(), 1u);
}

TEST_F(ReubicacionTest, ToggleConcurrenteConHilosSinPerderIncidentes) {
    central.registrarUnidad(std::make_shared<Ambulancia>(1, "AMB", 0));
    central.setTickMs(1);
    for (int i = 0; i < 6; ++i) central.recibirIncidente(incidente(i, 3));
    central.iniciar();
    std::thread cambios([&] {
        for (int i = 0; i < 50; ++i) {
            central.setReubicacionPreventiva(i % 2 == 0);
            central.comparacionPrediccion();
            std::this_thread::yield();
        }
    });
    cambios.join();
    const auto limite = std::chrono::steady_clock::now() + std::chrono::seconds(5);
    while (central.totalIncidentesAtendidos() < 6 && std::chrono::steady_clock::now() < limite)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    central.detener();
    EXPECT_EQ(central.totalIncidentesAtendidos(), 6);
    EXPECT_TRUE(central.getIncidentesActivos().empty());
    EXPECT_TRUE(central.getIncidentesPendientes().empty());
}
