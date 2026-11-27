#pragma once
#include <QDialog>

class MapaArequipa;
class QComboBox;
class QLineEdit;
class QPushButton;
class QTableWidget;

class GestionUnidadesDialog : public QDialog {
    Q_OBJECT

public:
    explicit GestionUnidadesDialog(const MapaArequipa& mapa, QWidget* parent = nullptr);

signals:
    void flotaActualizada();

private slots:
    void onAgregar();
    void onModificar();
    void onEliminar();
    void onRestablecer();
    void onSeleccionCambiada();

private:
    void configurarUI();
    void recargarTabla();
    int idSeleccionado() const;
    int siguienteIdDisponible() const;
    QString generarNombrePorDefecto(int tipoIndex) const;

    const MapaArequipa& m_mapa;
    QTableWidget* m_tabla = nullptr;
    QLineEdit* m_txtNombre = nullptr;
    QComboBox* m_cbTipo = nullptr;
    QComboBox* m_cbZonaBase = nullptr;
    QPushButton* m_btnAgregar = nullptr;
    QPushButton* m_btnModificar = nullptr;
    QPushButton* m_btnEliminar = nullptr;
    QPushButton* m_btnRestablecer = nullptr;
};

