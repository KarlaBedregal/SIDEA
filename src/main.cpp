#include <QApplication>
#include <QColor>
#include <QPalette>
#include "gui/MainWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("SIDEA");
    app.setApplicationVersion("0.1.0");
    app.setStyle("Fusion");

    QPalette claro;
    claro.setColor(QPalette::Window, QColor("#F8FAFC"));
    claro.setColor(QPalette::WindowText, QColor("#0F172A"));
    claro.setColor(QPalette::Base, QColor("#FFFFFF"));
    claro.setColor(QPalette::AlternateBase, QColor("#F1F5F9"));
    claro.setColor(QPalette::Text, QColor("#0F172A"));
    claro.setColor(QPalette::Button, QColor("#E2E8F0"));
    claro.setColor(QPalette::ButtonText, QColor("#0F172A"));
    claro.setColor(QPalette::Highlight, QColor("#2563EB"));
    claro.setColor(QPalette::HighlightedText, QColor("#FFFFFF"));
    claro.setColor(QPalette::ToolTipBase, QColor("#FFFFFF"));
    claro.setColor(QPalette::ToolTipText, QColor("#0F172A"));
    app.setPalette(claro);

    MainWindow window;
    window.setWindowTitle("SIDEA — Sistema Inteligente de Despacho de Emergencias de Arequipa");
    window.show();

    return app.exec();
}
