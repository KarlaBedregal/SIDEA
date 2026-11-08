#include <gtest/gtest.h>
#include "core/Ambulancia.h"
#include "core/Bombero.h"
#include "core/Incidente.h"
#include "core/Serenazgo.h"
#include "core/UnidadFactory.h"

TEST(UnidadTest, AmbulanciaCreacion) {
    Ambulancia a(1, "AMB-01", 0);
    EXPECT_EQ(a.getId(), 1);
    EXPECT_EQ(a.getTipoUnidad(), "Ambulancia");
    EXPECT_TRUE(a.estaLibre());
    EXPECT_EQ(a.getNodoActual(), 0);
}

TEST(UnidadTest, BomberoCreacion) {
    Bombero b(2, "BOM-01", 3);
    EXPECT_EQ(b.getTipoUnidad(), "Bombero");
    EXPECT_EQ(b.getNodoBase(), 3);
}

TEST(UnidadTest, SerenazgoCreacion) {
    Serenazgo s(3, "SER-01", 5);
    EXPECT_EQ(s.getId(), 3);
    EXPECT_EQ(s.getTipoUnidad(), "Serenazgo");
    EXPECT_TRUE(s.estaLibre());
}

TEST(UnidadTest, AmbulanciaSoloAtiendeUrgenciasMedicas) {
    Ambulancia ambulancia(1, "AMB-01", 0);
    Incidente urgencia(1, TipoIncidente::URGENCIA_MEDICA, 2, PrioridadIncidente::ALTA);
    Incidente incendio(2, TipoIncidente::INCENDIO, 3, PrioridadIncidente::ALTA);

    ambulancia.atenderIncidente(urgencia);
    ambulancia.atenderIncidente(incendio);

    EXPECT_EQ(ambulancia.getIncidentesAtendidos(), 1);
}

TEST(UnidadTest, BomberoAtiendeIncendiosYAccidentes) {
    Bombero bombero(2, "BOM-01", 3);
    Incidente incendio(1, TipoIncidente::INCENDIO, 4, PrioridadIncidente::ALTA);
    Incidente accidente(2, TipoIncidente::ACCIDENTE, 5, PrioridadIncidente::MEDIA);
    Incidente robo(3, TipoIncidente::ROBO, 6, PrioridadIncidente::BAJA);

    bombero.atenderIncidente(incendio);
    bombero.atenderIncidente(accidente);
    bombero.atenderIncidente(robo);

    EXPECT_EQ(bombero.getIncidentesAtendidos(), 2);
}

TEST(UnidadTest, SerenazgoSoloAtiendeRobos) {
    Serenazgo serenazgo(3, "SER-01", 5);
    Incidente robo(1, TipoIncidente::ROBO, 2, PrioridadIncidente::ALTA);
    Incidente urgencia(2, TipoIncidente::URGENCIA_MEDICA, 3, PrioridadIncidente::ALTA);

    serenazgo.atenderIncidente(robo);
    serenazgo.atenderIncidente(urgencia);

    EXPECT_EQ(serenazgo.getIncidentesAtendidos(), 1);
}

TEST(UnidadTest, FactoryCreaTiposCorrecto) {
    auto u1 = UnidadFactory::crear(1, TipoUnidad::AMBULANCIA, "AMB-F1", 0);
    auto u2 = UnidadFactory::crear(2, TipoUnidad::BOMBERO, "BOM-F1", 1);
    auto u3 = UnidadFactory::crear(3, TipoUnidad::SERENAZGO, "SER-F1", 2);

    EXPECT_EQ(u1->getTipoUnidad(), "Ambulancia");
    EXPECT_EQ(u2->getTipoUnidad(), "Bombero");
    EXPECT_EQ(u3->getTipoUnidad(), "Serenazgo");
}
