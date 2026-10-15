#include <gtest/gtest.h>
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
