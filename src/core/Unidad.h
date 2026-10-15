#pragma once
#include <memory>
#include <string>
#include <vector>

class Incidente;
class EstadoUnidad;

class Unidad {
public:
    Unidad(int id, std::string nombre, int nodoBase);
    virtual ~Unidad() = default;

    virtual void atenderIncidente(const Incidente& inc) = 0;
    virtual std::string getTipoUnidad() const = 0;

    void cambiarEstado(std::unique_ptr<EstadoUnidad> nuevoEstado);
    void tick();

    int getId() const;
    std::string getNombre() const;
    int getNodoActual() const;
    int getNodoBase() const;
    bool estaLibre() const;
    std::string getEstadoNombre() const;
    int getIncidentesAtendidos() const;
    double getTiempoOcupado() const;

    void setRuta(std::vector<int> ruta);
    void asignarIncidente(std::shared_ptr<Incidente> inc);

protected:
    int m_id;
    std::string m_nombre;
    int m_nodoBase;
    int m_nodoActual;
    std::unique_ptr<EstadoUnidad> m_estado;
    std::vector<int> m_ruta;
    std::shared_ptr<Incidente> m_incidenteActual;
    int m_incidentesAtendidos = 0;
    double m_tiempoOcupado = 0.0;
};
