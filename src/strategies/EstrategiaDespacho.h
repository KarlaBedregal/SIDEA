#pragma once
#include <memory>
#include <vector>
#include <string>

class Unidad;
class Incidente;

class EstrategiaDespacho {
public:
    virtual ~EstrategiaDespacho() = default;

    virtual std::shared_ptr<Unidad> seleccionar(
        const std::vector<std::shared_ptr<Unidad>>& unidades,
        const Incidente& incidente) = 0;

    virtual std::string nombre() const = 0;
};
