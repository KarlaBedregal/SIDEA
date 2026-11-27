#include "GestionUnidadesDialog.h"
#include "core/CentralDespacho.h"
#include "core/Unidad.h"
#include "core/UnidadFactory.h"
#include "graph/MapaArequipa.h"
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <algorithm>
#include <unordered_set>

namespace {
TipoUnidad tipoDesdeIndice(int index) {
    switch (index) {
        case 1: return TipoUnidad::BOMBERO;
        case 2: return TipoUnidad::SERENAZGO;
        case 0:
        default: return TipoUnidad::AMBULANCIA;
    }
}

int indiceDesdeTipo(const std::string& tipo) {
    if (tipo == "Bombero") return 1;
    if (tipo == "Serenazgo") return 2;
    return 0;
}
}

GestionUnidadesDialog::GestionUnidadesDialog(const MapaArequipa& mapa, QWidget* parent)
    : QDialog(parent), m_mapa(mapa) {
    setWindowTitle("Gestionar flota de unidades");
    resize(680, 500);
    configurarUI();
    recargarTabla();
}

void GestionUnidadesDialog::configurarUI() {
    auto* mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);

    m_tabla = new QTableWidget(0, 5, this);
    m_tabla->setHorizontalHeaderLabels({"ID", "Nombre", "Tipo", "Zona base", "Estado"});
    m_tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_tabla->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    m_tabla->verticalHeader()->setVisible(false);
    m_tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tabla->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);

    auto* gbEdicion = new QGroupBox("Datos de la unidad", this);
    auto* form = new QFormLayout(gbEdicion);

    m_txtNombre = new QLineEdit(this);
    m_txtNombre->setPlaceholderText("Ej: AMB-04 (vacío = automático)");

    m_cbTipo = new QComboBox(this);
    m_cbTipo->addItems({"Ambulancia", "Bombero", "Serenazgo"});

    m_cbZonaBase = new QComboBox(this);
    for (int id = 0; id < m_mapa.totalNodos(); ++id) {
        m_cbZonaBase->addItem(
            QString("%1 - %2 (%3)")
                .arg(id)
                .arg(QString::fromStdString(m_mapa.nombreZona(id)),
                     QString::fromStdString(m_mapa.direccion(id))),
            id);
    }

    form->addRow("Nombre:", m_txtNombre);
    form->addRow("Tipo:", m_cbTipo);
    form->addRow("Zona base:", m_cbZonaBase);

    auto* accionesLayout = new QHBoxLayout();
    m_btnAgregar = new QPushButton("+ Agregar unidad", this);
    m_btnModificar = new QPushButton("Guardar cambios", this);
    m_btnEliminar = new QPushButton("Eliminar", this);
    m_btnRestablecer = new QPushButton("Restablecer (8)", this);
    auto* btnCerrar = new QPushButton("Cerrar", this);

    m_btnAgregar->setStyleSheet("background-color: #16A34A; color: white; font-weight: bold; padding: 6px 10px; border-radius: 4px;");
    m_btnModificar->setStyleSheet("background-color: #2563EB; color: white; font-weight: bold; padding: 6px 10px; border-radius: 4px;");
    m_btnEliminar->setStyleSheet("background-color: #DC2626; color: white; font-weight: bold; padding: 6px 10px; border-radius: 4px;");
    m_btnRestablecer->setStyleSheet("background-color: #E2E8F0; color: #0F172A; font-weight: bold; padding: 6px 10px; border-radius: 4px;");

    accionesLayout->addWidget(m_btnAgregar);
    accionesLayout->addWidget(m_btnModificar);
    accionesLayout->addWidget(m_btnEliminar);
    accionesLayout->addWidget(m_btnRestablecer);
    accionesLayout->addStretch();
    accionesLayout->addWidget(btnCerrar);

    mainLayout->addWidget(new QLabel("Selecciona una unidad para modificarla o eliminarla, o agrega nuevas unidades:", this));
    mainLayout->addWidget(m_tabla, 1);
    mainLayout->addWidget(gbEdicion);
    mainLayout->addLayout(accionesLayout);

    connect(m_tabla, &QTableWidget::itemSelectionChanged,
            this, &GestionUnidadesDialog::onSeleccionCambiada);
    connect(m_btnAgregar, &QPushButton::clicked, this, &GestionUnidadesDialog::onAgregar);
    connect(m_btnModificar, &QPushButton::clicked, this, &GestionUnidadesDialog::onModificar);
    connect(m_btnEliminar, &QPushButton::clicked, this, &GestionUnidadesDialog::onEliminar);
    connect(m_btnRestablecer, &QPushButton::clicked, this, &GestionUnidadesDialog::onRestablecer);
    connect(btnCerrar, &QPushButton::clicked, this, &QDialog::accept);
}

void GestionUnidadesDialog::recargarTabla() {
    const auto unidades = CentralDespacho::instancia().getUnidades();
    m_tabla->setRowCount(static_cast<int>(unidades.size()));

    for (int fila = 0; fila < static_cast<int>(unidades.size()); ++fila) {
        const auto& u = unidades[static_cast<std::size_t>(fila)];
        auto* itemId = new QTableWidgetItem(QString::number(u->getId()));
        itemId->setData(Qt::UserRole, u->getId());
        itemId->setTextAlignment(Qt::AlignCenter);

        auto* itemNombre = new QTableWidgetItem(QString::fromStdString(u->getNombre()));
        auto* itemTipo = new QTableWidgetItem(QString::fromStdString(u->getTipoUnidad()));
        const int base = u->getNodoBase();
        auto* itemZona = new QTableWidgetItem(
            QString("%1 - %2").arg(base).arg(QString::fromStdString(m_mapa.nombreZona(base))));
        itemZona->setToolTip(QString::fromStdString(m_mapa.direccion(base)));
        auto* itemEstado = new QTableWidgetItem(QString::fromStdString(u->getEstadoNombre()));

        m_tabla->setItem(fila, 0, itemId);
        m_tabla->setItem(fila, 1, itemNombre);
        m_tabla->setItem(fila, 2, itemTipo);
        m_tabla->setItem(fila, 3, itemZona);
        m_tabla->setItem(fila, 4, itemEstado);
    }

    const bool haySeleccion = idSeleccionado() >= 0;
    m_btnModificar->setEnabled(haySeleccion);
    m_btnEliminar->setEnabled(haySeleccion);
}

int GestionUnidadesDialog::idSeleccionado() const {
    const auto items = m_tabla->selectedItems();
    if (items.isEmpty()) return -1;
    const int fila = items.first()->row();
    const auto* itemId = m_tabla->item(fila, 0);
    return itemId ? itemId->data(Qt::UserRole).toInt() : -1;
}

int GestionUnidadesDialog::siguienteIdDisponible() const {
    int maxId = 0;
    for (const auto& u : CentralDespacho::instancia().getUnidades()) {
        if (u) maxId = std::max(maxId, u->getId());
    }
    return maxId + 1;
}

QString GestionUnidadesDialog::generarNombrePorDefecto(int tipoIndex) const {
    const QString prefijo = (tipoIndex == 1) ? "BOM" : (tipoIndex == 2 ? "SER" : "AMB");
    std::unordered_set<std::string> usados;
    for (const auto& u : CentralDespacho::instancia().getUnidades()) {
        if (u) usados.insert(u->getNombre());
    }
    for (int n = 1; n < 1000; ++n) {
        const QString candidato = QString("%1-%2").arg(prefijo).arg(n, 2, 10, QChar('0'));
        if (usados.find(candidato.toStdString()) == usados.end()) return candidato;
    }
    return prefijo + "-99";
}

void GestionUnidadesDialog::onSeleccionCambiada() {
    const int id = idSeleccionado();
    m_btnModificar->setEnabled(id >= 0);
    m_btnEliminar->setEnabled(id >= 0);
    if (id < 0) return;

    for (const auto& u : CentralDespacho::instancia().getUnidades()) {
        if (u && u->getId() == id) {
            m_txtNombre->setText(QString::fromStdString(u->getNombre()));
            m_cbTipo->setCurrentIndex(indiceDesdeTipo(u->getTipoUnidad()));
            const int idxZona = m_cbZonaBase->findData(u->getNodoBase());
            if (idxZona >= 0) m_cbZonaBase->setCurrentIndex(idxZona);
            break;
        }
    }
}

void GestionUnidadesDialog::onAgregar() {
    const int tipoIdx = m_cbTipo->currentIndex();
    QString nombre = m_txtNombre->text().trimmed();
    if (nombre.isEmpty()) nombre = generarNombrePorDefecto(tipoIdx);
    const int nodoBase = m_cbZonaBase->currentData().toInt();
    const int nuevoId = siguienteIdDisponible();

    auto unidad = UnidadFactory::crear(
        nuevoId, tipoDesdeIndice(tipoIdx), nombre.toStdString(), nodoBase);
    CentralDespacho::instancia().registrarUnidad(std::move(unidad));

    m_txtNombre->clear();
    recargarTabla();
    emit flotaActualizada();
}

void GestionUnidadesDialog::onModificar() {
    const int id = idSeleccionado();
    if (id < 0) return;

    const int tipoIdx = m_cbTipo->currentIndex();
    QString nombre = m_txtNombre->text().trimmed();
    if (nombre.isEmpty()) nombre = generarNombrePorDefecto(tipoIdx);
    const int nodoBase = m_cbZonaBase->currentData().toInt();

    auto& central = CentralDespacho::instancia();
    central.eliminarUnidad(id);
    central.registrarUnidad(
        UnidadFactory::crear(id, tipoDesdeIndice(tipoIdx), nombre.toStdString(), nodoBase));

    recargarTabla();
    emit flotaActualizada();
}

void GestionUnidadesDialog::onEliminar() {
    const int id = idSeleccionado();
    if (id < 0) return;

    CentralDespacho::instancia().eliminarUnidad(id);
    m_txtNombre->clear();
    recargarTabla();
    emit flotaActualizada();
}

void GestionUnidadesDialog::onRestablecer() {
    auto& central = CentralDespacho::instancia();
    for (const auto& u : central.getUnidades()) {
        if (u) central.eliminarUnidad(u->getId());
    }

    central.registrarUnidad(UnidadFactory::crear(1, TipoUnidad::AMBULANCIA, "AMB-01", 0));
    central.registrarUnidad(UnidadFactory::crear(2, TipoUnidad::AMBULANCIA, "AMB-02", 4));
    central.registrarUnidad(UnidadFactory::crear(3, TipoUnidad::AMBULANCIA, "AMB-03", 9));
    central.registrarUnidad(UnidadFactory::crear(4, TipoUnidad::BOMBERO, "BOM-01", 1));
    central.registrarUnidad(UnidadFactory::crear(5, TipoUnidad::BOMBERO, "BOM-02", 3));
    central.registrarUnidad(UnidadFactory::crear(6, TipoUnidad::SERENAZGO, "SER-01", 2));
    central.registrarUnidad(UnidadFactory::crear(7, TipoUnidad::SERENAZGO, "SER-02", 6));
    central.registrarUnidad(UnidadFactory::crear(8, TipoUnidad::SERENAZGO, "SER-03", 8));

    m_txtNombre->clear();
    recargarTabla();
    emit flotaActualizada();
}

