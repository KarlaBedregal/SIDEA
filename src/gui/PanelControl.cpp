#include "PanelControl.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>

PanelControl::PanelControl(QWidget* parent) : QWidget(parent) {
    configurarUI();
    marcarDetenido();
}

void PanelControl::configurarUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(12);

    auto* gbSimulacion = new QGroupBox("Control de Simulación", this);
    auto* simLayout = new QVBoxLayout(gbSimulacion);

    m_lblEstado = new QLabel("Estado: Detenido", this);
    m_lblEstado->setStyleSheet("font-weight: bold; color: #2c3e50; font-size: 13px;");

    auto* velLayout = new QHBoxLayout();
    velLayout->addWidget(new QLabel("Velocidad:", this));
    m_sbVelocidad = new QSpinBox(this);
    m_sbVelocidad->setRange(100, 5000);
    m_sbVelocidad->setSingleStep(100);
    m_sbVelocidad->setValue(500);
    m_sbVelocidad->setSuffix(" ms/tick");
    velLayout->addWidget(m_sbVelocidad);

    auto* btnLayout = new QHBoxLayout();
    m_btnIniciar = new QPushButton("Iniciar", this);
    m_btnPausar  = new QPushButton("Pausar", this);
    m_btnDetener = new QPushButton("Detener", this);
    m_btnIniciar->setStyleSheet(
        "QPushButton{background:#27ae60;color:white;font-weight:bold;padding:6px;border-radius:4px;}"
        "QPushButton:disabled{background:#95a5a6;}");
    m_btnPausar->setStyleSheet(
        "QPushButton{background:#e67e22;color:white;font-weight:bold;padding:6px;border-radius:4px;}"
        "QPushButton:disabled{background:#95a5a6;}");
    m_btnDetener->setStyleSheet(
        "QPushButton{background:#c0392b;color:white;font-weight:bold;padding:6px;border-radius:4px;}"
        "QPushButton:disabled{background:#95a5a6;}");
    btnLayout->addWidget(m_btnIniciar);
    btnLayout->addWidget(m_btnPausar);
    btnLayout->addWidget(m_btnDetener);

    simLayout->addWidget(m_lblEstado);
    simLayout->addLayout(velLayout);
    simLayout->addLayout(btnLayout);

    auto* gbEstrategia = new QGroupBox("Estrategia de Despacho", this);
    auto* estLayout = new QVBoxLayout(gbEstrategia);
    m_cbEstrategia = new QComboBox(this);
    m_cbEstrategia->addItem("Más Cercana");
    m_cbEstrategia->addItem("Prioridad");
    m_cbEstrategia->addItem("Balance de Carga");
    estLayout->addWidget(m_cbEstrategia);

    mainLayout->addWidget(gbSimulacion);
    mainLayout->addWidget(gbEstrategia);
    mainLayout->addStretch();

    connect(m_btnIniciar, &QPushButton::clicked, this, &PanelControl::iniciarPresionado);
    connect(m_btnPausar,  &QPushButton::clicked, this, &PanelControl::pausarPresionado);
    connect(m_btnDetener, &QPushButton::clicked, this, &PanelControl::detenerPresionado);
    connect(m_cbEstrategia, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PanelControl::estrategiaChanged);
}

void PanelControl::marcarIniciado() {
    m_btnIniciar->setEnabled(false);
    m_btnIniciar->setText("Iniciar");
    m_btnPausar->setEnabled(true);
    m_btnDetener->setEnabled(true);
    m_lblEstado->setText("Estado: En ejecución");
    m_lblEstado->setStyleSheet("font-weight:bold;color:#27ae60;font-size:13px;");
}

void PanelControl::marcarPausado() {
    m_btnIniciar->setEnabled(true);
    m_btnIniciar->setText("Reanudar");
    m_btnPausar->setEnabled(false);
    m_btnDetener->setEnabled(true);
    m_lblEstado->setText("Estado: En pausa");
    m_lblEstado->setStyleSheet("font-weight:bold;color:#e67e22;font-size:13px;");
}

void PanelControl::marcarDetenido() {
    m_btnIniciar->setEnabled(true);
    m_btnIniciar->setText("Iniciar");
    m_btnPausar->setEnabled(false);
    m_btnDetener->setEnabled(false);
    m_lblEstado->setText("Estado: Detenido");
    m_lblEstado->setStyleSheet("font-weight:bold;color:#c0392b;font-size:13px;");
}

int PanelControl::estrategiaSeleccionada() const {
    return m_cbEstrategia ? m_cbEstrategia->currentIndex() : 0;
}

int PanelControl::velocidadSimulacion() const {
    return m_sbVelocidad ? m_sbVelocidad->value() : 500;
}
