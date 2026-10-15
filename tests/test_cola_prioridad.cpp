#include <gtest/gtest.h>
#include "graph/ColaPrioridad.h"

TEST(ColaPrioridadTest, OrdenCorrecto) {
    ColaPrioridad<int> cola(std::greater<int>{});
    cola.push(3);
    cola.push(1);
    cola.push(5);
    cola.push(2);

    EXPECT_EQ(cola.pop(), 5);
    EXPECT_EQ(cola.pop(), 3);
    EXPECT_EQ(cola.pop(), 2);
    EXPECT_EQ(cola.pop(), 1);
}

TEST(ColaPrioridadTest, PopEnColaVaciaLanzaExcepcion) {
    ColaPrioridad<int> cola;
    EXPECT_THROW(cola.pop(), std::underflow_error);
}

TEST(ColaPrioridadTest, Tamanio) {
    ColaPrioridad<int> cola;
    EXPECT_TRUE(cola.estaVacia());
    cola.push(1);
    EXPECT_EQ(cola.tamanio(), 1);
}
