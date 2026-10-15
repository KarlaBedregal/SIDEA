#include <gtest/gtest.h>
#include "strategies/EstrategiaMasCercana.h"
#include "strategies/EstrategiaPrioridad.h"
#include "strategies/EstrategiaBalanceCarga.h"
#include "core/Ambulancia.h"
#include "core/Incidente.h"
#include "graph/MapaArequipa.h"

class EstrategiaTest : public ::testing::Test {
protected:
    void SetUp() override {
        u1 = std::make_shared<Ambulancia>(1, "AMB-01", 0);
        u2 = std::make_shared<Ambulancia>(2, "AMB-02", 10);
        unidades = {u1, u2};
        inc = std::make_shared<Incidente>(1, TipoIncidente::URGENCIA_MEDICA,
                                           5, PrioridadIncidente::ALTA);
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
