#pragma once
#include <memory>
#include <string>
#include "Unidad.h"

enum class TipoUnidad { AMBULANCIA, BOMBERO, SERENAZGO };

class UnidadFactory {
public:
    static std::shared_ptr<Unidad> crear(int id, TipoUnidad tipo,
                                          const std::string& nombre, int nodoBase);
};
