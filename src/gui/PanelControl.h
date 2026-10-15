#pragma once
#include <QWidget>
#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>
#include <QLabel>

class PanelControl : public QWidget {
    Q_OBJECT

public:
    explicit PanelControl(QWidget* parent = nullptr);

    int estrategiaSeleccionada() const;
    int velocidadSimulacion() const;

signals:
    void iniciarPresionado();
    void pausarPresionado();
    void detenerPresionado();
    void estrategiaChanged(int index);

private:
    void configurarUI();

    QPushButton* m_btnIniciar;
    QPushButton* m_btnPausar;
    QPushButton* m_btnDetener;
    QComboBox* m_cbEstrategia;
    QSpinBox* m_sbVelocidad;
    QLabel* m_lblEstado;
};
