#include <gtest/gtest.h>
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QPixmap>
#include <QPushButton>
#include <QSpinBox>
#include <QThread>
#include "core/Ambulancia.h"
#include "gui/MapaWidget.h"
#include "gui/MainWindow.h"
#include "core/CentralDespacho.h"
#include "core/Incidente.h"
#include "graph/MapaArequipa.h"

TEST(MapaGuiTest, RecursosPngRealesDe32Pixels) {
    for (const auto* nombre : {"ambulancia", "bombero", "serenazgo", "incendio",
                               "accidente", "robo", "urgencia_medica"}) {
        QPixmap icono(QString(":/icons/%1.png").arg(nombre));
        ASSERT_FALSE(icono.isNull()) << nombre;
        EXPECT_EQ(icono.size(), QSize(32, 32));
    }
}

TEST(MapaGuiTest, IncidentesSeDibujanYDesaparecenAlAtenderse) {
    MapaArequipa mapa;
    MapaWidget widget(mapa);
    widget.resize(900, 650);
    const auto vacio = widget.grab().toImage();
    std::vector<std::shared_ptr<Incidente>> incidentes;
    for (int i = 0; i < 4; ++i)
        incidentes.push_back(std::make_shared<Incidente>(i, static_cast<TipoIncidente>(i),
                                                        i, PrioridadIncidente::ALTA));
    widget.setIncidentes(incidentes);
    EXPECT_NE(widget.grab().toImage(), vacio);
    for (const auto& inc : incidentes) inc->marcarAtendido();
    EXPECT_EQ(widget.grab().toImage(), vacio);
    widget.setIncidentes({nullptr});
    EXPECT_EQ(widget.grab().toImage(), vacio);
}

TEST(MapaGuiTest, MovimientoInterpoladoYRetiradaDeUnidades) {
    MapaArequipa mapa;
    MapaWidget widget(mapa);
    widget.resize(900, 650);
    widget.setDuracionMovimiento(200);
    auto unidad = std::make_shared<Ambulancia>(1, "AMB", 0);
    widget.setUnidades({unidad});
    const auto inicio = widget.grab().toImage();
    unidad->avanzarNodo(4);
    QThread::msleep(20);
    QApplication::processEvents(); // detecta nuevo nodo, progreso inicial cero
    const auto origen = widget.grab().toImage();
    EXPECT_EQ(origen, inicio);
    QThread::msleep(60);
    QApplication::processEvents();
    const auto intermedio = widget.grab().toImage();
    EXPECT_NE(intermedio, inicio);
    QThread::msleep(220);
    QApplication::processEvents();
    const auto llegada = widget.grab().toImage();
    EXPECT_NE(llegada, intermedio);
    widget.setUnidades({});
    EXPECT_NE(widget.grab().toImage(), llegada);
}

TEST(MainWindowGuiTest, InicioPausaReanudacionToggleYReinicio) {
    auto& central = CentralDespacho::instancia();
    central.reiniciar();
    {
        MainWindow window;
        auto* iniciar = window.findChild<QPushButton*>("iniciarSimulacion");
        auto* pausar = window.findChild<QPushButton*>("pausarSimulacion");
        auto* prediccion = window.findChild<QCheckBox*>("reubicacionPreventiva");
        ASSERT_NE(iniciar, nullptr); ASSERT_NE(pausar, nullptr); ASSERT_NE(prediccion, nullptr);
        window.findChild<QSpinBox*>("velocidadSimulacion")->setValue(100);
        EXPECT_EQ(central.getUnidades().size(), 8u);
        iniciar->click(); EXPECT_FALSE(iniciar->isEnabled());
        prediccion->setChecked(true); EXPECT_TRUE(central.reubicacionPreventiva());
        window.findChild<QComboBox*>("estrategiaDespacho")->setCurrentIndex(2);
        pausar->click(); EXPECT_TRUE(iniciar->isEnabled());
        iniciar->click(); EXPECT_FALSE(iniciar->isEnabled());
        window.findChild<QPushButton*>("detenerSimulacion")->click();
        EXPECT_TRUE(iniciar->isEnabled());
        window.findChild<QPushButton*>("nuevoEscenario")->click();
        EXPECT_FALSE(central.reubicacionPreventiva());
        EXPECT_TRUE(central.getIncidentesActivos().empty());
        EXPECT_TRUE(central.comparacionPrediccion().empty());
    }
    central.reiniciar();
}

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
