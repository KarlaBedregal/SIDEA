#pragma once
#include <QObject>
#include "../gui/MainWindow.h"
#include "CentralDespacho.h"

class ControladorDespacho : public QObject {
    Q_OBJECT
public:
    ControladorDespacho(MainWindow* vista, QObject* parent = nullptr)
        : QObject(parent), m_vista(vista) 
    {
        connect(m_vista, &MainWindow::simulacionIniciada, this, &ControladorDespacho::onIniciar);
        connect(m_vista, &MainWindow::simulacionDetenida, this, &ControladorDespacho::onDetener);
    }

private slots:
    void onIniciar() {
        CentralDespacho::instancia().iniciar();
    }
    
    void onDetener() {
        CentralDespacho::instancia().detener();
    }

private:
    MainWindow* m_vista;
};