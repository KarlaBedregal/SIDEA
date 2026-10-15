#include "PanelControl.h"
#include <QVBoxLayout>
#include <QGroupBox>

PanelControl::PanelControl(QWidget* parent) : QWidget(parent) {
    configurarUI();
}

void PanelControl::configurarUI() {
    auto* layout = new QVBoxLayout(this);

    m_btnIniciar  = new QPushButton("Iniciar");
    m_btnPausar   = new QPushButton("Pausar");
    m_btnDetener  = new QPushButton("Detener");
    m_cbEstrategia = new QComboBox();
    m_cbEstrategia->addItems({"Unidad más cercana", "Mayor prioridad", "Balance de carga"});
    m_sbVelocidad = new QSpinBox();
    m_sbVelocidad->setRange(100, 5000);
    m_sbVelocidad->setValue(1000);
    m_sbVelocidad->setSuffix(" ms/tick");
    m_lblEstado = new QLabel("Estado: detenido");

    layout->addWidget(m_lblEstado);
    layout->addWidget(m_cbEstrategia);
    layout->addWidget(m_sbVelocidad);
    layout->addWidget(m_btnIniciar);
    layout->addWidget(m_btnPausar);
    layout->addWidget(m_btnDetener);

    connect(m_btnIniciar,  &QPushButton::clicked, this, &PanelControl::iniciarPresionado);
    connect(m_btnPausar,   &QPushButton::clicked, this, &PanelControl::pausarPresionado);
    connect(m_btnDetener,  &QPushButton::clicked, this, &PanelControl::detenerPresionado);
    connect(m_cbEstrategia, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PanelControl::estrategiaChanged);
}

int PanelControl::estrategiaSeleccionada() const { return m_cbEstrategia->currentIndex(); }
int PanelControl::velocidadSimulacion() const { return m_sbVelocidad->value(); }
