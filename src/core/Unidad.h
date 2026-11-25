#pragma once
#include <chrono>
#include <cstddef>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

class Incidente;
class EstadoUnidad;
class MapaArequipa;

class Unidad {
public:
    Unidad(int id, std::string nombre, int nodoBase);
    virtual ~Unidad();

    virtual void atenderIncidente(const Incidente& inc) = 0;
    virtual std::string getTipoUnidad() const = 0;

    void cambiarEstado(std::unique_ptr<EstadoUnidad> nuevoEstado);
    void tick();
    void avanzarNodo(int nodo);

    int getId() const;
    std::string getNombre() const;
    int getNodoActual() const;
    int getNodoBase() const;
    bool estaLibre() const;
    std::string getEstadoNombre() const;
    int getIncidentesAtendidos() const;
    double getTiempoOcupado() const;
    bool puedeAtender(const Incidente& inc) const;

    void setRuta(std::vector<int> ruta);
    // Reserva la unidad; false si está ocupada o el incidente/ruta no es válido.
    bool asignarIncidente(std::shared_ptr<Incidente> inc);
    // Recorrido preventivo interrumpible: la unidad sigue disponible para despachos.
    bool reubicar(int destino);
    void cancelarReubicacion();

private:
    friend class EstadoLibre;
    friend class EstadoEnRuta;
    friend class EstadoAtendiendo;
    friend class EstadoRegresando;
    bool prepararRuta(int destino);
    bool avanzarEnRuta();
    std::unique_ptr<EstadoUnidad> m_estadoPendiente;
    bool m_enTick = false;
    bool m_reubicando = false;
    std::size_t m_indiceRuta = 0;
    int m_ticksAtencionRestantes = 0;
    std::chrono::steady_clock::time_point m_inicioOcupacion;
    std::unique_ptr<const MapaArequipa> m_mapa;

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
    mutable std::recursive_mutex m_mutex;
};
