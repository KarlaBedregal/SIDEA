#pragma once
#include <unordered_map>
#include <vector>
#include <limits>
#include <queue>
#include "Grafo.h"

struct ResultadoDijkstra {
    std::unordered_map<int, double> distancias;
    std::unordered_map<int, int> anteriores;

    std::vector<int> reconstruirRuta(int destino) const;
    double distanciaA(int destino) const;
    bool alcanzable(int destino) const;
};

template<typename T>
ResultadoDijkstra dijkstra(const Grafo<T>& grafo, int origen) {
    constexpr double INF = std::numeric_limits<double>::infinity();
    ResultadoDijkstra resultado;

    using Par = std::pair<double, int>;
    std::priority_queue<Par, std::vector<Par>, std::greater<Par>> pq;

    for (int nodo : grafo.nodos()) {
        resultado.distancias[nodo] = INF;
        resultado.anteriores[nodo] = -1;
    }

    resultado.distancias[origen] = 0.0;
    pq.push({0.0, origen});

    while (!pq.empty()) {
        auto [dist, u] = pq.top(); pq.pop();
        if (dist > resultado.distancias[u]) continue;

        for (const auto& arista : grafo.vecinos(u)) {
            double nueva = dist + arista.peso;
            if (nueva < resultado.distancias[arista.destino]) {
                resultado.distancias[arista.destino] = nueva;
                resultado.anteriores[arista.destino] = u;
                pq.push({nueva, arista.destino});
            }
        }
    }

    return resultado;
}
