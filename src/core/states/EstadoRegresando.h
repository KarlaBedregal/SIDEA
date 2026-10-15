#pragma once
#include "EstadoUnidad.h"

class EstadoRegresando : public EstadoUnidad {
public:
    void manejar(Unidad& unidad) override;
    std::string nombre() const override { return "Regresando"; }
};
