#pragma once
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>

class PanelControl : public QWidget {
    Q_OBJECT

public:
    explicit PanelControl(QWidget* parent = nullptr);

    int estrategiaSeleccionada() const;
    int velocidadSimulacion() const;

    void setConteoUnidades(int ambulancias, int bomberos, int serenazgos);
    void marcarIniciado();
    void marcarPausado();
    void marcarDetenido();

signals:
    void iniciarPresionado();
    void pausarPresionado();
    void detenerPresionado();
    void estrategiaChanged(int index);
    void reubicacionChanged(bool activa);
    void reiniciarPresionado();
    void gestionarUnidadesPresionado();
    void cantidadTipoCambiada(int tipoIndex, int nuevaCantidad);

private:
    void configurarUI();

    QPushButton* m_btnIniciar = nullptr;
    QPushButton* m_btnPausar = nullptr;
    QPushButton* m_btnDetener = nullptr;
    QPushButton* m_btnNuevo = nullptr;
    QPushButton* m_btnGestionarUnidades = nullptr;
    QComboBox* m_cbEstrategia = nullptr;
    QSpinBox* m_sbVelocidad = nullptr;
    QSpinBox* m_sbAmbulancias = nullptr;
    QSpinBox* m_sbBomberos = nullptr;
    QSpinBox* m_sbSerenazgos = nullptr;
    QLabel* m_lblEstado = nullptr;
    QCheckBox* m_cbPrediccion = nullptr;
};
