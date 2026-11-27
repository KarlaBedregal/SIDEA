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

private:
    void configurarUI();

    QPushButton* m_btnIniciar = nullptr;
    QPushButton* m_btnPausar = nullptr;
    QPushButton* m_btnDetener = nullptr;
    QPushButton* m_btnNuevo = nullptr;
    QComboBox* m_cbEstrategia = nullptr;
    QSpinBox* m_sbVelocidad = nullptr;
    QLabel* m_lblEstado = nullptr;
    QCheckBox* m_cbPrediccion = nullptr;
};
