// Capturas reales de los widgets Qt; no se construyen imágenes de una UI ficticia.
#include <QApplication>
#include <QCheckBox>
#include <QDir>
#include <QFontDatabase>
#include <QTimer>
#include "gui/MainWindow.h"
#include "core/CentralDespacho.h"
#include "core/Incidente.h"
#include "core/Unidad.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);
    app.setStyle("Fusion");
    // En offscreen puede no estar disponible la fuente del escritorio.
    // Se permite una fuente local explícita para exportar texto legible.
    if (argc > 2) {
        const int id = QFontDatabase::addApplicationFont(QString::fromLocal8Bit(argv[2]));
        const auto familias = QFontDatabase::applicationFontFamilies(id);
        if (familias.isEmpty()) return 2;
        app.setFont(QFont(familias.front(), 10));
        QFont::insertSubstitution("Sans Serif", familias.front());
    }
    const QString destino = argc > 1 ? QString::fromLocal8Bit(argv[1]) : "docs/capturas";
    QDir().mkpath(destino);
    auto& central = CentralDespacho::instancia();
    central.reiniciar();
    MainWindow window;
    window.show();
    // Escenario fijo para que se vean los cuatro tipos y un caso esperando recurso.
    for (int i = 0; i < 4; ++i)
        central.recibirIncidente(std::make_shared<Incidente>(1000 + i,
            static_cast<TipoIncidente>(i), i + 10, PrioridadIncidente::ALTA));
    bool correcto = true;
    QTimer::singleShot(150, [&] {
        correcto &= window.grab().save(destino + "/incidentes.png");
        // Primer ciclo sin prevención, avanzando el mismo motor de estados.
        for (int paso = 0; paso < 60; ++paso) {
            central.procesarCola();
            for (const auto& unidad : central.getUnidades()) unidad->tick();
        }
        window.findChild<QCheckBox*>("reubicacionPreventiva")->setChecked(true);
        for (int i = 0; i < 4; ++i)
            central.recibirIncidente(std::make_shared<Incidente>(2000 + i,
                static_cast<TipoIncidente>(i), i + 10, PrioridadIncidente::ALTA));
        for (int paso = 0; paso < 60; ++paso) {
            central.procesarCola();
            for (const auto& unidad : central.getUnidades()) unidad->tick();
        }
        QMetaObject::invokeMethod(&window, "onTickSimulacion", Qt::DirectConnection);
        QTimer::singleShot(700, [&] {
            correcto &= window.grab().save(destino + "/comparacion.png");
            app.exit(correcto ? 0 : 1);
        });
    });
    const int resultado = app.exec();
    return resultado;
}
