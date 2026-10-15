#include <gtest/gtest.h>
#include "core/Incidente.h"

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
