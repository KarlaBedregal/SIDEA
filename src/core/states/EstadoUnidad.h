#pragma once
#include <string>

class Unidad;

class EstadoUnidad {
public:
    virtual ~EstadoUnidad() = default;
    virtual void manejar(Unidad& unidad) = 0;
    virtual std::string nombre() const = 0;
};
