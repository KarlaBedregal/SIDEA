#pragma once
#include "EstrategiaDespacho.h"

class EstrategiaBalanceCarga : public EstrategiaDespacho {
public:
    std::shared_ptr<Unidad> seleccionar(
        const std::vector<std::shared_ptr<Unidad>>& unidades,
        const Incidente& incidente) override;

    std::string nombre() const override { return "Balance de carga"; }
};
