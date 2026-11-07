#include <gtest/gtest.h>
#include <algorithm>
#include "graph/Grafo.h"

TEST(GrafoTest, AgregarNodosYAristas) {
    Grafo<std::string> g;
    g.agregarNodo(0);
    g.agregarNodo(1);
    g.agregarAristaBidireccional(0, 1, 5.0, "Av. Principal");

    EXPECT_TRUE(g.existeNodo(0));
    EXPECT_TRUE(g.existeNodo(1));
    EXPECT_EQ(g.vecinos(0).size(), 1u);
    EXPECT_EQ(g.vecinos(0)[0].destino, 1);
    EXPECT_DOUBLE_EQ(g.vecinos(0)[0].peso, 5.0);
    EXPECT_EQ(g.vecinos(0)[0].datos, "Av. Principal");
    ASSERT_EQ(g.vecinos(1).size(), 1u);
    EXPECT_EQ(g.vecinos(1)[0].destino, 0);
    EXPECT_DOUBLE_EQ(g.vecinos(1)[0].peso, 5.0);
    EXPECT_EQ(g.vecinos(1)[0].datos, "Av. Principal");
}

TEST(GrafoTest, NodoInexistenteLanzaExcepcion) {
    Grafo<int> g;
    EXPECT_THROW(g.vecinos(99), std::out_of_range);
}

TEST(GrafoTest, TotalNodos) {
    Grafo<int> g;
    g.agregarNodo(0);
    g.agregarNodo(1);
    g.agregarNodo(2);
    EXPECT_EQ(g.totalNodos(), 3);
}

TEST(GrafoTest, AristaDirigidaRegistraAmbosExtremos) {
    Grafo<int> g;
    g.agregarArista(4, 9, 2.5);

    EXPECT_EQ(g.totalNodos(), 2);
    ASSERT_EQ(g.vecinos(4).size(), 1u);
    EXPECT_EQ(g.vecinos(4)[0].destino, 9);
    EXPECT_EQ(g.vecinos(4)[0].datos, 0);
    EXPECT_TRUE(g.vecinos(9).empty());
    auto nodos = g.nodos();
    std::sort(nodos.begin(), nodos.end());
    EXPECT_EQ(nodos, (std::vector<int>{4, 9}));
}

TEST(GrafoTest, AgregarNodoRepetidoConservaAristas) {
    Grafo<int> g;
    g.agregarAristaBidireccional(0, 1, 3.0);
    g.agregarNodo(0);

    EXPECT_EQ(g.totalNodos(), 2);
    ASSERT_EQ(g.vecinos(0).size(), 1u);
    EXPECT_EQ(g.vecinos(0)[0].destino, 1);
}
