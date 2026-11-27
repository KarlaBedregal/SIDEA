#include "LogPanel.h"
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDateTime>
#include <QListWidgetItem>

LogPanel::LogPanel(QWidget* parent) : QWidget(parent) {
    configurarUI();
}

void LogPanel::configurarUI() {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);
    layout->setSpacing(0);

    auto* gb = new QGroupBox("Log de eventos", this);
    auto* gbLayout = new QVBoxLayout(gb);
    gbLayout->setContentsMargins(4, 6, 4, 4);

    m_lista = new QListWidget(this);
    m_lista->setStyleSheet(
        "QListWidget { background: #0d1117; border: none; font-family: monospace; font-size: 11px; color: #c9d1d9; }"
        "QListWidget::item { padding: 2px 4px; border-bottom: 1px solid #21262d; }"
        "QListWidget::item:selected { background: #1f2937; }");
    m_lista->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_lista->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_lista->setWordWrap(false);
    m_lista->setFocusPolicy(Qt::NoFocus);

    gbLayout->addWidget(m_lista);
    layout->addWidget(gb);
}

void LogPanel::agregarEvento(const QString& texto, const QString& tipo) {
    const QString hora = QDateTime::currentDateTime().toString("hh:mm:ss");
    const QString linea = QStringLiteral("[%1] %2").arg(hora, texto);

    auto* item = new QListWidgetItem(linea, m_lista);

    if (tipo == "incidente")
        item->setForeground(QColor("#FF8C00"));
    else if (tipo == "asignacion")
        item->setForeground(QColor("#3fb950"));
    else if (tipo == "libre")
        item->setForeground(QColor("#58a6ff"));
    else if (tipo == "atendido")
        item->setForeground(QColor("#a371f7"));
    else
        item->setForeground(QColor("#8b949e"));

    while (m_lista->count() > MAX_ENTRADAS)
        delete m_lista->takeItem(0);

    m_lista->scrollToBottom();
}
