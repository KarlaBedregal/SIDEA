#pragma once
#include "EstadoUnidad.h"

class EstadoLibre : public EstadoUnidad {
public:
    void manejar(Unidad& unidad) override;
    std::string nombre() const override { return "Libre"; }
};
