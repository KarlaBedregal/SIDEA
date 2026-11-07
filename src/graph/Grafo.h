#pragma once
#include <unordered_map>
#include <vector>
#include <stdexcept>

template<typename T>
struct Arista {
    int destino;
    double peso;
    T datos;
};

template<typename T>
class Grafo {
public:
    void agregarNodo(int id);
    // Crea los extremos si todavía no existen, conservando sus aristas previas.
    void agregarArista(int origen, int destino, double peso, T datos = T{});
    void agregarAristaBidireccional(int origen, int destino, double peso, T datos = T{});

    const std::vector<Arista<T>>& vecinos(int nodo) const;
    bool existeNodo(int id) const;
    std::vector<int> nodos() const;
    int totalNodos() const;

private:
    std::unordered_map<int, std::vector<Arista<T>>> m_adyacencia;
};

// --- Implementación (debe estar en el .h por ser template) ---

template<typename T>
void Grafo<T>::agregarNodo(int id) {
    if (!existeNodo(id)) m_adyacencia[id] = {};
}

template<typename T>
void Grafo<T>::agregarArista(int origen, int destino, double peso, T datos) {
    agregarNodo(origen);
    agregarNodo(destino);
    m_adyacencia[origen].push_back({destino, peso, datos});
}

template<typename T>
void Grafo<T>::agregarAristaBidireccional(int origen, int destino, double peso, T datos) {
    agregarArista(origen, destino, peso, datos);
    agregarArista(destino, origen, peso, datos);
}

template<typename T>
const std::vector<Arista<T>>& Grafo<T>::vecinos(int nodo) const {
    auto it = m_adyacencia.find(nodo);
    if (it == m_adyacencia.end()) throw std::out_of_range("Nodo no existe");
    return it->second;
}

template<typename T>
bool Grafo<T>::existeNodo(int id) const {
    return m_adyacencia.count(id) > 0;
}

template<typename T>
std::vector<int> Grafo<T>::nodos() const {
    std::vector<int> resultado;
    resultado.reserve(m_adyacencia.size());
    for (const auto& [id, _] : m_adyacencia) resultado.push_back(id);
    return resultado;
}

template<typename T>
int Grafo<T>::totalNodos() const {
    return static_cast<int>(m_adyacencia.size());
}
