#pragma once
#include "EstadoUnidad.h"

class EstadoEnRuta : public EstadoUnidad {
public:
    void manejar(Unidad& unidad) override;
    std::string nombre() const override { return "En Ruta"; }
};
