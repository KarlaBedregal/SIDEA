#include <gtest/gtest.h>
#include <thread>
#include <chrono>
#include <atomic>
#include "patterns/IObserver.h"
#include "patterns/IObservable.h"

// Observable concreto de prueba (no usa el singleton CentralDespacho)
class ObservableTest : public IObservable {
public:
    void disparar(const std::string& evento) { notificar(evento); }
};

class MockObserver : public IObserver {
public:
    std::vector<std::string> eventos;
    void actualizar(const std::string& e) override { eventos.push_back(e); }
};

TEST(ObserverTest, SuscribirYRecibirEventos) {
    ObservableTest obs;
    auto mock = std::make_shared<MockObserver>();
    obs.suscribir(mock);

    obs.disparar("incidente_recibido");
    obs.disparar("unidad_asignada");

    ASSERT_EQ(mock->eventos.size(), 2u);
    EXPECT_EQ(mock->eventos[0], "incidente_recibido");
    EXPECT_EQ(mock->eventos[1], "unidad_asignada");
}

TEST(ObserverTest, Desuscribir) {
    ObservableTest obs;
    auto mock = std::make_shared<MockObserver>();
    obs.suscribir(mock);
    obs.desuscribir(mock);

    obs.disparar("evento");
    EXPECT_TRUE(mock->eventos.empty());
}

TEST(ObserverTest, MultiplesObservadores) {
    ObservableTest obs;
    auto m1 = std::make_shared<MockObserver>();
    auto m2 = std::make_shared<MockObserver>();
    obs.suscribir(m1);
    obs.suscribir(m2);

    obs.disparar("unidad_libre");

    EXPECT_EQ(m1->eventos.size(), 1u);
    EXPECT_EQ(m2->eventos.size(), 1u);
}

TEST(ObserverTest, TresEventosDespacho) {
    ObservableTest obs;
    auto mock = std::make_shared<MockObserver>();
    obs.suscribir(mock);

    obs.disparar("incidente_recibido");
    obs.disparar("unidad_asignada");
    obs.disparar("unidad_libre");

    ASSERT_EQ(mock->eventos.size(), 3u);
    EXPECT_EQ(mock->eventos[2], "unidad_libre");
}

TEST(ObserverTest, NotificarDesdeDosHilos) {
    ObservableTest obs;
    std::atomic<int> contador{0};

    class CountObserver : public IObserver {
    public:
        std::atomic<int>& c;
        explicit CountObserver(std::atomic<int>& ref) : c(ref) {}
        void actualizar(const std::string&) override { ++c; }
    };

    obs.suscribir(std::make_shared<CountObserver>(contador));

    std::thread t1([&obs]() {
        for (int i = 0; i < 50; ++i) obs.disparar("evento_a");
    });
    std::thread t2([&obs]() {
        for (int i = 0; i < 50; ++i) obs.disparar("evento_b");
    });
    t1.join();
    t2.join();

    EXPECT_EQ(contador.load(), 100);
}
