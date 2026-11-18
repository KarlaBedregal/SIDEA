#pragma once
#include <QObject>
#include <QDebug>
#include <memory>
#include "../gui/MainWindow.h"
#include "CentralDespacho.h"
#include "../graph/MapaArequipa.h"
#include "../strategies/EstrategiaMasCercana.h"
#include "../strategies/EstrategiaPrioridad.h"
#include "../strategies/EstrategiaBalanceCarga.h"

class ControladorDespacho : public QObject {
    Q_OBJECT
public:
    ControladorDespacho(MainWindow* vista, QObject* parent = nullptr)
        : QObject(parent), m_vista(vista)
    {
        connect(m_vista, &MainWindow::simulacionIniciada, this, &ControladorDespacho::onIniciar);
        connect(m_vista, &MainWindow::simulacionPausada, this, &ControladorDespacho::onPausar);
        connect(m_vista, &MainWindow::simulacionDetenida, this, &ControladorDespacho::onDetener);
        connect(m_vista, &MainWindow::estrategiaCambiada, this, &ControladorDespacho::onCambiarEstrategia);
    }

private slots:
    void onIniciar() {
        CentralDespacho::instancia().iniciar();
        qDebug() << "[Controlador] Simulacion iniciada";
    }
    void onPausar() {
        qDebug() << "[Controlador] Pausa pendiente: el Modelo aun no la implementa";
    }
    void onDetener() {
        CentralDespacho::instancia().detener();
        qDebug() << "[Controlador] Simulacion detenida";
    }
    void onCambiarEstrategia(int index) {
        auto& central = CentralDespacho::instancia();
        switch (index) {
        case 0: central.setEstrategia(std::make_unique<EstrategiaMasCercana>(m_mapa)); break;
        case 1: central.setEstrategia(std::make_unique<EstrategiaPrioridad>()); break;
        case 2: central.setEstrategia(std::make_unique<EstrategiaBalanceCarga>()); break;
        }
        qDebug() << "[Controlador] Estrategia cambiada, indice:" << index;
    }

private:
    MainWindow* m_vista;
    MapaArequipa m_mapa;
};