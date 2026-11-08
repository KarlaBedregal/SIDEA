#include <QApplication>
#include "gui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("SIDEA");
    app.setApplicationVersion("0.1.0");

    MainWindow window;
    window.setWindowTitle("SIDEA");
    window.show();

    return app.exec();
}
