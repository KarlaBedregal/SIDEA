#pragma once
#include <unordered_map>
#include <vector>
#include <limits>
#include <queue>
#include <algorithm>
#include "Grafo.h"

struct ResultadoDijkstra {
    std::unordered_map<int, double> distancias;
    std::unordered_map<int, int> anteriores;

    double distanciaA(int destino) const {
        auto it = distancias.find(destino);
        if (it == distancias.end()) return std::numeric_limits<double>::infinity();
        return it->second;
    }

    bool alcanzable(int destino) const {
        return distanciaA(destino) < std::numeric_limits<double>::infinity();
    }

    std::vector<int> reconstruirRuta(int destino) const {
        std::vector<int> ruta;
        int actual = destino;
        while (actual != -1) {
            ruta.push_back(actual);
            auto it = anteriores.find(actual);
            if (it == anteriores.end() || it->second == -1) break;
            actual = it->second;
        }
        std::reverse(ruta.begin(), ruta.end());
        return ruta;
    }
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
