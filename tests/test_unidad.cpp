#include <gtest/gtest.h>
#include "core/Ambulancia.h"
#include "core/Bombero.h"
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

TEST(UnidadTest, FactoryCreaTiposCorrecto) {
    auto u1 = UnidadFactory::crear(1, TipoUnidad::AMBULANCIA, "AMB-F1", 0);
    auto u2 = UnidadFactory::crear(2, TipoUnidad::BOMBERO, "BOM-F1", 1);
    auto u3 = UnidadFactory::crear(3, TipoUnidad::SERENAZGO, "SER-F1", 2);

    EXPECT_EQ(u1->getTipoUnidad(), "Ambulancia");
    EXPECT_EQ(u2->getTipoUnidad(), "Bombero");
    EXPECT_EQ(u3->getTipoUnidad(), "Serenazgo");
}
