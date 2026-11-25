#include <gtest/gtest.h>
#include "core/Incidente.h"
#include "core/IncidenteFactory.h"
#include <limits>
#include <stdexcept>

TEST(IncidenteTest, ConstruccionCorrecta) {
    Incidente inc(1, TipoIncidente::INCENDIO, 5, PrioridadIncidente::ALTA);
    EXPECT_EQ(inc.getId(), 1);
    EXPECT_EQ(inc.getTipo(), TipoIncidente::INCENDIO);
    EXPECT_EQ(inc.getNodoUbicacion(), 5);
    EXPECT_EQ(inc.getPrioridad(), PrioridadIncidente::ALTA);
    EXPECT_FALSE(inc.estaAtendido());
}

TEST(IncidenteTest, MarcarAtendido) {
    Incidente inc(2, TipoIncidente::ROBO, 3, PrioridadIncidente::MEDIA);
    inc.marcarAtendido();
    EXPECT_TRUE(inc.estaAtendido());
}

TEST(IncidenteTest, TipoToString) {
    Incidente inc(3, TipoIncidente::URGENCIA_MEDICA, 1, PrioridadIncidente::CRITICA);
    EXPECT_EQ(inc.tipoToString(), "Urgencia médica");
    EXPECT_EQ(inc.prioridadToString(), "Crítica");
}

TEST(IncidenteFactoryTest, CrearConservaDatosYEstadoInicial) {
    auto incidente = IncidenteFactory::crear(
        42, TipoIncidente::ACCIDENTE, 7, PrioridadIncidente::CRITICA);

    ASSERT_NE(incidente, nullptr);
    EXPECT_EQ(incidente->getId(), 42);
    EXPECT_EQ(incidente->getTipo(), TipoIncidente::ACCIDENTE);
    EXPECT_EQ(incidente->getNodoUbicacion(), 7);
    EXPECT_EQ(incidente->getPrioridad(), PrioridadIncidente::CRITICA);
    EXPECT_FALSE(incidente->estaAtendido());
}

TEST(IncidenteFactoryTest, AleatorioRespetaRangosYConservaId) {
    // Se verifican invariantes, sin depender de una secuencia aleatoria concreta.
    for (int nodoMax : {1, 19, std::numeric_limits<int>::max()}) {
        for (int id = 0; id < 200; ++id) {
            SCOPED_TRACE(::testing::Message() << "nodoMax=" << nodoMax << ", id=" << id);
            auto incidente = IncidenteFactory::crearAleatorio(id, nodoMax);

            ASSERT_NE(incidente, nullptr);
            EXPECT_EQ(incidente->getId(), id);
            EXPECT_GE(incidente->getNodoUbicacion(), 0);
            EXPECT_LE(incidente->getNodoUbicacion(), nodoMax);
            EXPECT_GE(static_cast<int>(incidente->getTipo()),
                      static_cast<int>(TipoIncidente::INCENDIO));
            EXPECT_LE(static_cast<int>(incidente->getTipo()),
                      static_cast<int>(TipoIncidente::URGENCIA_MEDICA));
            EXPECT_GE(static_cast<int>(incidente->getPrioridad()),
                      static_cast<int>(PrioridadIncidente::BAJA));
            EXPECT_LE(static_cast<int>(incidente->getPrioridad()),
                      static_cast<int>(PrioridadIncidente::CRITICA));
            EXPECT_FALSE(incidente->estaAtendido());
        }
    }
}

TEST(IncidenteFactoryTest, AleatorioConNodoMaximoCeroSoloUsaNodoCero) {
    for (int id = 0; id < 20; ++id) {
        auto incidente = IncidenteFactory::crearAleatorio(id, 0);
        ASSERT_NE(incidente, nullptr);
        EXPECT_EQ(incidente->getNodoUbicacion(), 0);
    }
}

TEST(IncidenteFactoryTest, AleatorioRechazaNodoMaximoNegativo) {
    EXPECT_THROW(IncidenteFactory::crearAleatorio(1, -1), std::invalid_argument);
    EXPECT_THROW(IncidenteFactory::crearAleatorio(1, std::numeric_limits<int>::min()),
                 std::invalid_argument);
}
