#include "PanelControl.h"
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QSignalBlocker>
#include <QVBoxLayout>

PanelControl::PanelControl(QWidget* parent) : QWidget(parent) {
    configurarUI();
    marcarDetenido();
}

void PanelControl::configurarUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(8, 8, 8, 8);
    mainLayout->setSpacing(10);

    auto* gbSimulacion = new QGroupBox("Control de Simulación", this);
    auto* simLayout = new QVBoxLayout(gbSimulacion);

    m_lblEstado = new QLabel("Estado: Detenido", this);
    m_lblEstado->setStyleSheet("font-weight: bold; color: #2c3e50; font-size: 13px;");

    auto* velLayout = new QHBoxLayout();
    velLayout->addWidget(new QLabel("Velocidad:", this));
    m_sbVelocidad = new QSpinBox(this);
    m_sbVelocidad->setObjectName("velocidadSimulacion");
    m_sbVelocidad->setRange(100, 5000);
    m_sbVelocidad->setSingleStep(100);
    m_sbVelocidad->setValue(500);
    m_sbVelocidad->setSuffix(" ms/tick");
    velLayout->addWidget(m_sbVelocidad);

    auto* btnLayout = new QHBoxLayout();
    m_btnIniciar = new QPushButton("Iniciar", this);
    m_btnPausar  = new QPushButton("Pausar", this);
    m_btnDetener = new QPushButton("Detener", this);
    m_btnIniciar->setObjectName("iniciarSimulacion");
    m_btnPausar->setObjectName("pausarSimulacion");
    m_btnDetener->setObjectName("detenerSimulacion");
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

    m_btnNuevo = new QPushButton("Nuevo escenario", this);
    m_btnNuevo->setObjectName("nuevoEscenario");

    simLayout->addWidget(m_lblEstado);
    simLayout->addLayout(velLayout);
    simLayout->addLayout(btnLayout);
    simLayout->addWidget(m_btnNuevo);

    auto* gbFlota = new QGroupBox("Flota de Unidades", this);
    auto* flotaLayout = new QVBoxLayout(gbFlota);
    flotaLayout->setSpacing(6);

    auto* gridFlota = new QGridLayout();
    auto crearSpin = [this](const char* objName, int inicial) {
        auto* sb = new QSpinBox(this);
        sb->setObjectName(objName);
        sb->setRange(0, 15);
        sb->setValue(inicial);
        sb->setAlignment(Qt::AlignCenter);
        return sb;
    };

    m_sbAmbulancias = crearSpin("cantidadAmbulancias", 3);
    m_sbBomberos    = crearSpin("cantidadBomberos", 2);
    m_sbSerenazgos  = crearSpin("cantidadSerenazgos", 3);

    gridFlota->addWidget(new QLabel("Ambulancias:", this), 0, 0);
    gridFlota->addWidget(m_sbAmbulancias, 0, 1);
    gridFlota->addWidget(new QLabel("Bomberos:", this), 1, 0);
    gridFlota->addWidget(m_sbBomberos, 1, 1);
    gridFlota->addWidget(new QLabel("Serenazgo:", this), 2, 0);
    gridFlota->addWidget(m_sbSerenazgos, 2, 1);

    m_btnGestionarUnidades = new QPushButton("Editar / personalizar unidades...", this);
    m_btnGestionarUnidades->setObjectName("gestionarUnidades");
    m_btnGestionarUnidades->setStyleSheet(
        "QPushButton{background:#E2E8F0;color:#0F172A;font-weight:bold;padding:5px;border:1px solid #CBD5E1;border-radius:4px;}"
        "QPushButton:hover{background:#E0F2FE;border-color:#0284C7;color:#0369A1;}");

    flotaLayout->addLayout(gridFlota);
    flotaLayout->addWidget(m_btnGestionarUnidades);

    auto* gbEstrategia = new QGroupBox("Estrategia de Despacho", this);
    auto* estLayout = new QVBoxLayout(gbEstrategia);
    m_cbEstrategia = new QComboBox(this);
    m_cbEstrategia->setObjectName("estrategiaDespacho");
    m_cbEstrategia->addItem("Más Cercana");
    m_cbEstrategia->addItem("Prioridad");
    m_cbEstrategia->addItem("Balance de Carga");
    estLayout->addWidget(m_cbEstrategia);

    m_cbPrediccion = new QCheckBox("Reubicación preventiva", this);
    m_cbPrediccion->setObjectName("reubicacionPreventiva");

    mainLayout->addWidget(gbSimulacion);
    mainLayout->addWidget(gbFlota);
    mainLayout->addWidget(gbEstrategia);
    mainLayout->addWidget(m_cbPrediccion);
    mainLayout->addStretch();

    connect(m_btnIniciar, &QPushButton::clicked, this, &PanelControl::iniciarPresionado);
    connect(m_btnPausar,  &QPushButton::clicked, this, &PanelControl::pausarPresionado);
    connect(m_btnDetener, &QPushButton::clicked, this, &PanelControl::detenerPresionado);
    connect(m_btnNuevo,   &QPushButton::clicked, this, &PanelControl::reiniciarPresionado);
    connect(m_btnGestionarUnidades, &QPushButton::clicked, this, &PanelControl::gestionarUnidadesPresionado);
    connect(m_cbEstrategia, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &PanelControl::estrategiaChanged);
    connect(m_cbPrediccion, &QCheckBox::toggled, this, &PanelControl::reubicacionChanged);

    connect(m_sbAmbulancias, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int v) { emit cantidadTipoCambiada(0, v); });
    connect(m_sbBomberos, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int v) { emit cantidadTipoCambiada(1, v); });
    connect(m_sbSerenazgos, QOverload<int>::of(&QSpinBox::valueChanged),
            this, [this](int v) { emit cantidadTipoCambiada(2, v); });
}

void PanelControl::setConteoUnidades(int ambulancias, int bomberos, int serenazgos) {
    const QSignalBlocker b1(m_sbAmbulancias);
    const QSignalBlocker b2(m_sbBomberos);
    const QSignalBlocker b3(m_sbSerenazgos);
    if (m_sbAmbulancias) m_sbAmbulancias->setValue(ambulancias);
    if (m_sbBomberos) m_sbBomberos->setValue(bomberos);
    if (m_sbSerenazgos) m_sbSerenazgos->setValue(serenazgos);
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
