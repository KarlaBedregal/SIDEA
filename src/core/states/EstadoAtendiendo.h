#pragma once
#include "EstadoUnidad.h"

class EstadoAtendiendo : public EstadoUnidad {
public:
    void manejar(Unidad& unidad) override;
    std::string nombre() const override { return "Atendiendo"; }
};
