#include "UnidadFactory.h"
#include "Ambulancia.h"
#include "Bombero.h"
#include "Serenazgo.h"
#include <stdexcept>

std::shared_ptr<Unidad> UnidadFactory::crear(int id, TipoUnidad tipo,
                                               const std::string& nombre, int nodoBase) {
    switch (tipo) {
        case TipoUnidad::AMBULANCIA: return std::make_shared<Ambulancia>(id, nombre, nodoBase);
        case TipoUnidad::BOMBERO:    return std::make_shared<Bombero>(id, nombre, nodoBase);
        case TipoUnidad::SERENAZGO:  return std::make_shared<Serenazgo>(id, nombre, nodoBase);
    }
    throw std::invalid_argument("Tipo de unidad desconocido");
}
