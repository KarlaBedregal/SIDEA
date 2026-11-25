#include <gtest/gtest.h>
#include "core/Ambulancia.h"
#include "core/Incidente.h"
#include "core/states/EstadoUnidad.h"
#include <algorithm>
#include <vector>

namespace {
auto incidente(int nodo = 3, PrioridadIncidente prioridad = PrioridadIncidente::ALTA) {
    return std::make_shared<Incidente>(1, TipoIncidente::URGENCIA_MEDICA, nodo, prioridad);
}
}

TEST(EstadosTest, CicloCompletoSigueRutaYAtiendeUnaVez) {
    Ambulancia unidad(1, "AMB", 0);
    auto inc = incidente();
    ASSERT_TRUE(unidad.asignarIncidente(inc));
    EXPECT_FALSE(unidad.estaLibre());
    std::vector<std::string> fases;
    std::vector<int> recorrido;
    for (int i = 0; i < 50 && !unidad.estaLibre(); ++i) {
        unidad.tick();
        fases.push_back(unidad.getEstadoNombre());
        recorrido.push_back(unidad.getNodoActual());
    }
    EXPECT_TRUE(unidad.estaLibre());
    EXPECT_TRUE(inc->estaAtendido());
    EXPECT_EQ(unidad.getIncidentesAtendidos(), 1);
    EXPECT_EQ(unidad.getNodoActual(), 0);
    EXPECT_GT(unidad.getTiempoOcupado(), 0.0);
    fases.erase(std::unique(fases.begin(), fases.end()), fases.end());
    EXPECT_EQ(fases, (std::vector<std::string>{"EnRuta", "Atendiendo", "Regresando", "Libre"}));
    recorrido.erase(std::unique(recorrido.begin(), recorrido.end()), recorrido.end());
    EXPECT_EQ(recorrido, (std::vector<int>{0, 4, 2, 3, 2, 4, 0}));
    for (int i = 0; i < 5; ++i) unidad.tick();
    EXPECT_EQ(unidad.getIncidentesAtendidos(), 1);
}

TEST(EstadosTest, ReservaImpideReemplazarIncidenteAntesDelPrimerTick) {
    Ambulancia unidad(1, "AMB", 0);
    auto primero = incidente();
    auto segundo = incidente(1);
    ASSERT_TRUE(unidad.asignarIncidente(primero));
    EXPECT_FALSE(unidad.asignarIncidente(segundo));
    for (int i = 0; i < 50 && !unidad.estaLibre(); ++i) unidad.tick();
    EXPECT_TRUE(primero->estaAtendido());
    EXPECT_FALSE(segundo->estaAtendido());
    EXPECT_TRUE(unidad.asignarIncidente(segundo));
}

TEST(EstadosTest, IncidenteEnLaBaseTambienCompletaElCiclo) {
    Ambulancia unidad(1, "AMB", 0);
    auto inc = incidente(0, PrioridadIncidente::CRITICA);
    ASSERT_TRUE(unidad.asignarIncidente(inc));
    unidad.tick(); EXPECT_EQ(unidad.getEstadoNombre(), "EnRuta");
    unidad.tick(); EXPECT_EQ(unidad.getEstadoNombre(), "Atendiendo");
    unidad.tick(); EXPECT_EQ(unidad.getEstadoNombre(), "Regresando");
    unidad.tick(); EXPECT_TRUE(unidad.estaLibre());
    EXPECT_EQ(unidad.getNodoActual(), 0);
    EXPECT_TRUE(inc->estaAtendido());
}

TEST(EstadosTest, DuracionDeAtencionDependeDeLaPrioridad) {
    for (int prioridad = 1; prioridad <= 4; ++prioridad) {
        Ambulancia unidad(1, "AMB", 0);
        ASSERT_TRUE(unidad.asignarIncidente(incidente(0, static_cast<PrioridadIncidente>(prioridad))));
        unidad.tick(); unidad.tick();
        int ticks = 0;
        while (unidad.getEstadoNombre() == "Atendiendo" && ticks < 10) {
            unidad.tick(); ++ticks;
        }
        EXPECT_EQ(ticks, 5 - prioridad);
    }
}

TEST(EstadosTest, RechazaIncidentesNulosAtendidosONodosInvalidos) {
    Ambulancia unidad(1, "AMB", 0);
    EXPECT_FALSE(unidad.asignarIncidente(nullptr));
    EXPECT_FALSE(unidad.asignarIncidente(incidente(-1)));
    EXPECT_FALSE(unidad.asignarIncidente(incidente(20)));
    auto atendido = incidente(); atendido->marcarAtendido();
    EXPECT_FALSE(unidad.asignarIncidente(atendido));
    EXPECT_TRUE(unidad.estaLibre());
    unidad.tick();
    EXPECT_EQ(unidad.getNodoActual(), 0);
}

TEST(EstadosTest, RechazaTipoIncompatibleYPrioridadInvalida) {
    Ambulancia unidad(1, "AMB", 0);
    auto incendio = std::make_shared<Incidente>(2, TipoIncidente::INCENDIO, 1,
                                               PrioridadIncidente::ALTA);
    EXPECT_FALSE(unidad.asignarIncidente(incendio));
    EXPECT_FALSE(unidad.asignarIncidente(incidente(1, static_cast<PrioridadIncidente>(9))));
    EXPECT_TRUE(unidad.estaLibre());
}

TEST(EstadosTest, TransicionNoDestruyeElEstadoDuranteManejar) {
    bool destruido = false;
    class EstadoSiguiente : public EstadoUnidad {
    public:
        void manejar(Unidad&) override {}
        std::string nombre() const override { return "Siguiente"; }
    };
    class EstadoTransicion : public EstadoUnidad {
        bool& m_destruido;
    public:
        explicit EstadoTransicion(bool& destruido) : m_destruido(destruido) {}
        ~EstadoTransicion() override { m_destruido = true; }
        void manejar(Unidad& unidad) override {
            unidad.cambiarEstado(std::make_unique<EstadoSiguiente>());
            EXPECT_FALSE(m_destruido);
        }
        std::string nombre() const override { return "Anterior"; }
    };
    Ambulancia unidad(1, "AMB", 0);
    unidad.cambiarEstado(std::make_unique<EstadoTransicion>(destruido));
    unidad.tick();
    EXPECT_TRUE(destruido);
    EXPECT_EQ(unidad.getEstadoNombre(), "Siguiente");
}
