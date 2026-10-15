#pragma once
#include "Unidad.h"

class Bombero : public Unidad {
public:
    Bombero(int id, std::string nombre, int nodoBase);

    void atenderIncidente(const Incidente& inc) override;
    std::string getTipoUnidad() const override;
};
