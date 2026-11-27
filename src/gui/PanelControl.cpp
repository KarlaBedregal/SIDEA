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

    // Grupo 1: Controles de Simulación
    auto* gbSimulacion = new QGroupBox("Control de Simulación", this);
    auto* simLayout = new QVBoxLayout(gbSimulacion);

    m_lblEstado = new QLabel("Estado: Detenido", this);
    m_lblEstado->setStyleSheet("font-weight: bold; color: #2c3e50; font-size: 13px;");

    auto* velLayout = new QHBoxLayout();
    auto* lblVel = new QLabel("Velocidad:", this);
    m_sbVelocidad = new QSpinBox(this);
    m_sbVelocidad->setRange(100, 5000);
    m_sbVelocidad->setSingleStep(100);
    m_sbVelocidad->setValue(500);
    m_sbVelocidad->setSuffix(" ms/tick");
    velLayout->addWidget(lblVel);
    velLayout->addWidget(m_sbVelocidad);

    auto* btnLayout = new QHBoxLayout();
    m_btnIniciar  = new QPushButton("Iniciar", this);
    m_btnPausar   = new QPushButton("Pausar", this);
    m_btnDetener  = new QPushButton("Detener", this);
    m_btnIniciar->setObjectName("iniciarSimulacion");
    m_btnPausar->setObjectName("pausarSimulacion");
    m_btnDetener->setObjectName("detenerSimulacion");
    m_sbVelocidad->setObjectName("velocidadSimulacion");

    btnLayout->addWidget(m_btnIniciar);
    btnLayout->addWidget(m_btnPausar);
    btnLayout->addWidget(m_btnDetener);

    simLayout->addWidget(m_lblEstado);
    simLayout->addLayout(velLayout);
    simLayout->addLayout(btnLayout);

    // Grupo 2: Estrategia de Despacho
    auto* gbEstrategia = new QGroupBox("Estrategia de Despacho", this);
    auto* estLayout = new QVBoxLayout(gbEstrategia);

    m_cbEstrategia = new QComboBox(this);
    m_cbEstrategia->setObjectName("estrategiaDespacho");
    m_cbEstrategia->addItem("Más Cercana");      // index 0
    m_cbEstrategia->addItem("Prioridad");        // index 1
    m_cbEstrategia->addItem("Balance de Carga"); // index 2
    estLayout->addWidget(m_cbEstrategia);
    m_cbPrediccion = new QCheckBox("Reubicación preventiva", this);
    m_cbPrediccion->setObjectName("reubicacionPreventiva");
    m_cbPrediccion->setToolTip("Las unidades libres recorren la ruta a la zona de mayor riesgo.\n"
                             "Un nuevo despacho interrumpe este recorrido.");
    estLayout->addWidget(m_cbPrediccion);
    auto* nuevo = new QPushButton("Nuevo escenario", this);
    nuevo->setObjectName("nuevoEscenario");
    estLayout->addWidget(nuevo);
    connect(nuevo, &QPushButton::clicked, this, &PanelControl::reiniciarPresionado);
    connect(m_cbPrediccion, &QCheckBox::toggled, this, &PanelControl::reubicacionChanged);
    connect(nuevo, &QPushButton::clicked, this, [this] { m_cbPrediccion->setChecked(false); });

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
    m_sbVelocidad->setEnabled(false);
    m_btnIniciar->setEnabled(false);
    m_btnIniciar->setText("Iniciar");
    m_btnPausar->setEnabled(true);
    m_btnPausar->setText("Pausar");
    m_btnDetener->setEnabled(true);
    m_lblEstado->setText("Estado: En ejecución");
}

void PanelControl::marcarPausado() {
    m_sbVelocidad->setEnabled(true);
    m_btnIniciar->setEnabled(true);
    m_btnIniciar->setText("Reanudar");
    m_btnPausar->setEnabled(false);
    m_btnDetener->setEnabled(true);
    m_lblEstado->setText("Estado: En pausa");
}

void PanelControl::marcarDetenido() {
    m_sbVelocidad->setEnabled(true);
    m_btnIniciar->setEnabled(true);
    m_btnIniciar->setText("Iniciar");
    m_btnPausar->setEnabled(false);
    m_btnDetener->setEnabled(false);
    m_lblEstado->setText("Estado: Detenido");
}

int PanelControl::estrategiaSeleccionada() const {
    return m_cbEstrategia ? m_cbEstrategia->currentIndex() : 0;
}

int PanelControl::velocidadSimulacion() const {
    return m_sbVelocidad ? m_sbVelocidad->value() : 500;
}
