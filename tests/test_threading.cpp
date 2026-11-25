#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include "core/Ambulancia.h"
#include "core/Bombero.h"
#include "core/Serenazgo.h"
#include "graph/MapaArequipa.h"
#include "threading/SimuladorThread.h"

TEST(SimuladorThreadTest, ArrancarDetener) {
    MapaArequipa mapa;
    auto amb = std::make_shared<Ambulancia>(1, "AMB-01", 0);

    SimuladorThread sim(amb, mapa, 50);
    sim.iniciar();
    EXPECT_TRUE(sim.estaActivo());

    std::this_thread::sleep_for(std::chrono::milliseconds(150));
    sim.detener();
    EXPECT_FALSE(sim.estaActivo());
}

TEST(SimuladorThreadTest, TresHilosConcurrentes) {
    MapaArequipa mapa;
    auto amb = std::make_shared<Ambulancia>(1, "AMB-01", 0);
    auto bom = std::make_shared<Bombero>(2, "BOM-01", 1);
    auto ser = std::make_shared<Serenazgo>(3, "SER-01", 2);

    SimuladorThread s1(amb, mapa, 50);
    SimuladorThread s2(bom, mapa, 50);
    SimuladorThread s3(ser, mapa, 50);

    s1.iniciar(); s2.iniciar(); s3.iniciar();

    EXPECT_TRUE(s1.estaActivo());
    EXPECT_TRUE(s2.estaActivo());
    EXPECT_TRUE(s3.estaActivo());

    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    s1.detener(); s2.detener(); s3.detener();

    EXPECT_FALSE(s1.estaActivo());
    EXPECT_FALSE(s2.estaActivo());
    EXPECT_FALSE(s3.estaActivo());
}

TEST(SimuladorThreadTest, DetenerSinIniciar) {
    MapaArequipa mapa;
    auto amb = std::make_shared<Ambulancia>(1, "AMB-01", 0);
    SimuladorThread sim(amb, mapa, 100);
    EXPECT_NO_THROW(sim.detener());
    EXPECT_FALSE(sim.estaActivo());
}

TEST(SimuladorThreadTest, AccesoConcurrenteAUnidad) {
    MapaArequipa mapa;
    auto amb = std::make_shared<Ambulancia>(1, "AMB-01", 0);

    SimuladorThread sim(amb, mapa, 20);
    sim.iniciar();

    // Leer desde el hilo principal mientras el worker llama tick() en loop
    for (int i = 0; i < 15; ++i) {
        EXPECT_TRUE(amb->estaLibre());
        EXPECT_EQ(amb->getNodoActual(), 0);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    sim.detener();
}

TEST(UnidadThreadSafetyTest, TickYGettersConcurrentes) {
    auto amb = std::make_shared<Ambulancia>(1, "AMB-01", 0);

    // Un hilo llama tick() en bucle; el hilo principal lee estado
    std::thread worker([&amb]() {
        for (int i = 0; i < 200; ++i) {
            amb->tick();
        }
    });

    for (int i = 0; i < 200; ++i) {
        (void)amb->estaLibre();
        (void)amb->getNodoActual();
        (void)amb->getEstadoNombre();
    }

    worker.join();
    // Sin incidente ni ruta asignada, la unidad debe seguir libre
    EXPECT_TRUE(amb->estaLibre());
    EXPECT_EQ(amb->getNodoActual(), 0);
}

TEST(UnidadThreadSafetyTest, SetRutaYAvanzarNodoConcurrente) {
    auto bom = std::make_shared<Bombero>(2, "BOM-01", 3);

    std::thread escritor([&bom]() {
        for (int i = 0; i < 50; ++i) {
            bom->setRuta({3, 5, 7, 9});
            bom->avanzarNodo(5);
            bom->avanzarNodo(3);
        }
    });

    for (int i = 0; i < 50; ++i) {
        (void)bom->getNodoActual();
        (void)bom->estaLibre();
    }

    escritor.join();
    // No debe haber crash ni deadlock
    SUCCEED();
}
