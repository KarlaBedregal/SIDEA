#pragma once
#include <memory>
#include <string>
#include "Unidad.h"

enum class TipoUnidad { AMBULANCIA, BOMBERO, SERENAZGO };

class UnidadFactory {
public:
    // Crea la subclase solicitada, libre y ubicada en nodoBase.
    // Lanza std::invalid_argument si tipo no corresponde a un TipoUnidad valido.
    static std::shared_ptr<Unidad> crear(int id, TipoUnidad tipo,
                                          const std::string& nombre, int nodoBase);
};
