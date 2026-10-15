#include "Dijkstra.h"
#include <algorithm>

std::vector<int> ResultadoDijkstra::reconstruirRuta(int destino) const {
    std::vector<int> ruta;
    for (int nodo = destino; nodo != -1; ) {
        ruta.push_back(nodo);
        auto it = anteriores.find(nodo);
        nodo = (it != anteriores.end()) ? it->second : -1;
    }
    std::reverse(ruta.begin(), ruta.end());
    return ruta;
}

double ResultadoDijkstra::distanciaA(int destino) const {
    auto it = distancias.find(destino);
    return (it != distancias.end()) ? it->second : std::numeric_limits<double>::infinity();
}

bool ResultadoDijkstra::alcanzable(int destino) const {
    return distanciaA(destino) < std::numeric_limits<double>::infinity();
}
