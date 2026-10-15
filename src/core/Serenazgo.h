#pragma once
#include "Unidad.h"

class Serenazgo : public Unidad {
public:
    Serenazgo(int id, std::string nombre, int nodoBase);

    void atenderIncidente(const Incidente& inc) override;
    std::string getTipoUnidad() const override;
};
