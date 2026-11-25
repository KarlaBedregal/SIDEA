#include <gtest/gtest.h>
#include <type_traits>
#include "strategies/EstrategiaDespacho.h"
#include "strategies/EstrategiaMasCercana.h"
#include "strategies/EstrategiaPrioridad.h"
#include "strategies/EstrategiaBalanceCarga.h"
#include "core/Ambulancia.h"
#include "core/Incidente.h"
#include "core/CentralDespacho.h"
#include "core/states/EstadoEnRuta.h"
#include "graph/MapaArequipa.h"

// 1. Verificación en tiempo de compilación: la interfaz debe ser abstracta y con destructor virtual
static_assert(std::is_abstract_v<EstrategiaDespacho>,
              "EstrategiaDespacho debe ser una interfaz abstracta pura (no instanciable)");
static_assert(std::has_virtual_destructor_v<EstrategiaDespacho>,
              "EstrategiaDespacho debe tener destructor virtual");

// Subclase espía para verificar la asignación de incidentes sin modificar la visibilidad de Unidad
class AmbulanciaSpy : public Ambulancia {
public:
    using Ambulancia::Ambulancia;
    std::shared_ptr<Incidente> getIncidenteAsignado() const {
        return m_incidenteActual;
    }
};

class EstrategiaTest : public ::testing::Test {
protected:
    void SetUp() override {
        // En MapaArequipa:
        // Nodo 0: Centro Histórico
        // Nodo 4: Yanahuara (distancia a nodo 0 = 1.8 km)
        // Nodo 10: Sachaca (distancia a nodo 0 = 3.4 km)
        u1 = std::make_shared<Ambulancia>(1, "AMB-01", 4);
        u2 = std::make_shared<Ambulancia>(2, "AMB-02", 10);
        unidades = {u1, u2};
        inc = std::make_shared<Incidente>(1, TipoIncidente::URGENCIA_MEDICA,
                                           0, PrioridadIncidente::ALTA);
    }

    std::shared_ptr<Ambulancia> u1, u2;
    std::vector<std::shared_ptr<Unidad>> unidades;
    std::shared_ptr<Incidente> inc;
    MapaArequipa mapa;
};

TEST_F(EstrategiaTest, PrioridadDevuelveUnidadLibre) {
    EstrategiaPrioridad ep;
    auto elegida = ep.seleccionar(unidades, *inc);
    EXPECT_NE(elegida, nullptr);
    EXPECT_TRUE(elegida->estaLibre());
}

TEST_F(EstrategiaTest, BalanceCargaDevuelveUnidadConMenorCarga) {
    EstrategiaBalanceCarga eb;
    auto elegida = eb.seleccionar(unidades, *inc);
    EXPECT_NE(elegida, nullptr);
}

TEST_F(EstrategiaTest, EstrategiaMasCercanaNombre) {
    EstrategiaMasCercana emc(mapa);
    EXPECT_EQ(emc.nombre(), "Unidad más cercana");
}

TEST_F(EstrategiaTest, EstrategiaMasCercanaSeleccionaUnidadMasCercana) {
    EstrategiaMasCercana emc(mapa);

    // u1 está en nodo 4 (distancia 1.8 a nodo 0)
    // u2 está en nodo 10 (distancia 3.4 a nodo 0)
    auto elegida = emc.seleccionar(unidades, *inc);
    ASSERT_NE(elegida, nullptr);
    EXPECT_EQ(elegida->getId(), 1);

    // Debe seleccionar la más cercana independientemente del orden en el vector
    std::vector<std::shared_ptr<Unidad>> unidadesInvertidas = {u2, u1};
    auto elegidaInvertida = emc.seleccionar(unidadesInvertidas, *inc);
    ASSERT_NE(elegidaInvertida, nullptr);
    EXPECT_EQ(elegidaInvertida->getId(), 1);
}

TEST_F(EstrategiaTest, EstrategiaMasCercanaIgnoraUnidadOcupada) {
    EstrategiaMasCercana emc(mapa);

    // u1 (nodo 4, más cercana a 1.8 km) pasa a estar ocupada ("En Ruta")
    u1->cambiarEstado(std::make_unique<EstadoEnRuta>());
    ASSERT_FALSE(u1->estaLibre());
    ASSERT_TRUE(u2->estaLibre());

    // Debe ignorar u1 y seleccionar u2 aunque esté más lejos (nodo 10, 3.4 km)
    auto elegida = emc.seleccionar(unidades, *inc);
    ASSERT_NE(elegida, nullptr);
    EXPECT_EQ(elegida->getId(), 2);
}

TEST_F(EstrategiaTest, EstrategiaMasCercanaSinUnidadesLibresDevuelveNullptr) {
    EstrategiaMasCercana emc(mapa);

    // Ambas unidades ocupadas
    u1->cambiarEstado(std::make_unique<EstadoEnRuta>());
    u2->cambiarEstado(std::make_unique<EstadoEnRuta>());
    ASSERT_FALSE(u1->estaLibre());
    ASSERT_FALSE(u2->estaLibre());

    auto elegida = emc.seleccionar(unidades, *inc);
    EXPECT_EQ(elegida, nullptr);

    // Lista vacía de unidades
    std::vector<std::shared_ptr<Unidad>> vacia;
    auto elegidaVacia = emc.seleccionar(vacia, *inc);
    EXPECT_EQ(elegidaVacia, nullptr);
}

TEST_F(EstrategiaTest, IntegracionCentralDespachoConEstrategiaMasCercana) {
    auto& central = CentralDespacho::instancia();
    central.reiniciar();

    central.setEstrategia(std::make_unique<EstrategiaMasCercana>(mapa));

    // ambLejana registrada primero en la central (nodo 10 -> dist 3.4 a nodo 0)
    // ambCercana registrada después (nodo 4 -> dist 1.8 a nodo 0)
    auto ambLejana = std::make_shared<AmbulanciaSpy>(10, "AMB-LEJANA", 10);
    auto ambCercana = std::make_shared<AmbulanciaSpy>(4, "AMB-CERCANA", 4);

    central.registrarUnidad(ambLejana);
    central.registrarUnidad(ambCercana);

    auto incCentro = std::make_shared<Incidente>(100, TipoIncidente::URGENCIA_MEDICA, 0, PrioridadIncidente::ALTA);
    central.recibirIncidente(incCentro);

    EXPECT_EQ(central.totalIncidentesPendientes(), 1);
    EXPECT_EQ(central.totalIncidentesAtendidos(), 0);

    // procesarCola debe ejecutar la estrategia configurada y despachar la unidad más cercana
    central.procesarCola();

    EXPECT_EQ(central.totalIncidentesPendientes(), 0);
    EXPECT_EQ(central.totalIncidentesAtendidos(), 0);

    // La unidad más cercana debe haber recibido el incidente asignado
    EXPECT_NE(ambCercana->getIncidenteAsignado(), nullptr);
    EXPECT_EQ(ambCercana->getIncidenteAsignado()->getId(), 100);

    // La unidad lejana no debe haber sido asignada
    EXPECT_EQ(ambLejana->getIncidenteAsignado(), nullptr);

    for (int i = 0; i < 30 && !ambCercana->estaLibre(); ++i) ambCercana->tick();
    EXPECT_TRUE(incCentro->estaAtendido());
    EXPECT_EQ(central.totalIncidentesAtendidos(), 1);

    central.reiniciar();
}
