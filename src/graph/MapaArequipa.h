#pragma once
#include <string>
#include <unordered_map>
#include "Grafo.h"

struct DatosVia {
    std::string nombre;
};

struct PosicionNodo {
    float x, y;
    std::string zona;
};

class MapaArequipa {
public:
    MapaArequipa();

    const Grafo<DatosVia>& grafo() const;
    PosicionNodo posicion(int nodo) const;
    std::string nombreZona(int nodo) const;
    int totalNodos() const;

private:
    void construir();

    Grafo<DatosVia> m_grafo;
    std::unordered_map<int, PosicionNodo> m_posiciones;
};
