#pragma once
#include "EstrategiaDespacho.h"
#include "graph/MapaArequipa.h"

class EstrategiaMasCercana : public EstrategiaDespacho {
public:
    explicit EstrategiaMasCercana(const MapaArequipa& mapa);

    std::shared_ptr<Unidad> seleccionar(
        const std::vector<std::shared_ptr<Unidad>>& unidades,
        const Incidente& incidente) override;

    std::string nombre() const override { return "Unidad más cercana"; }

private:
    const MapaArequipa& m_mapa;
};
