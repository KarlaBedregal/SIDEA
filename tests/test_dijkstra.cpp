#include <gtest/gtest.h>
#include "graph/Grafo.h"
#include "graph/Dijkstra.h"

class DijkstraTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Grafo: 0--2--1--3--2--3--4
        for (int i = 0; i <= 4; i++) g.agregarNodo(i);
        g.agregarAristaBidireccional(0, 1, 2.0, {});
        g.agregarAristaBidireccional(1, 2, 3.0, {});
        g.agregarAristaBidireccional(2, 3, 1.0, {});
        g.agregarAristaBidireccional(0, 3, 10.0, {});
    }
    Grafo<int> g;
};

TEST_F(DijkstraTest, DistanciaCorrecta) {
    auto res = dijkstra(g, 0);
    EXPECT_DOUBLE_EQ(res.distanciaA(1), 2.0);
    EXPECT_DOUBLE_EQ(res.distanciaA(2), 5.0);
    EXPECT_DOUBLE_EQ(res.distanciaA(3), 6.0);
}

TEST_F(DijkstraTest, RutaReconstruida) {
    auto res = dijkstra(g, 0);
    auto ruta = res.reconstruirRuta(3);
    ASSERT_EQ(ruta.size(), 4u);
    EXPECT_EQ(ruta[0], 0);
    EXPECT_EQ(ruta.back(), 3);
}

TEST_F(DijkstraTest, NodoInalcanzable) {
    auto res = dijkstra(g, 0);
    EXPECT_FALSE(res.alcanzable(4));
}
