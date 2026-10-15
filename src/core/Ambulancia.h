#pragma once
#include "Unidad.h"

class Ambulancia : public Unidad {
public:
    Ambulancia(int id, std::string nombre, int nodoBase);

    void atenderIncidente(const Incidente& inc) override;
    std::string getTipoUnidad() const override;
};
