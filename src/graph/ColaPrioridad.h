#pragma once
#include <vector>
#include <functional>
#include <stdexcept>

template<typename T>
class ColaPrioridad {
public:
    using Comparador = std::function<bool(const T&, const T&)>;

    explicit ColaPrioridad(Comparador cmp = std::less<T>{});

    void push(T elemento);
    T pop();
    const T& top() const;
    bool estaVacia() const;
    int tamanio() const;

private:
    std::vector<T> m_heap;
    Comparador m_cmp;

    void subirHeap(int idx);
    void bajarHeap(int idx);
};

// --- Implementación ---

template<typename T>
ColaPrioridad<T>::ColaPrioridad(Comparador cmp) : m_cmp(cmp) {}

template<typename T>
void ColaPrioridad<T>::push(T elemento) {
    m_heap.push_back(std::move(elemento));
    subirHeap(static_cast<int>(m_heap.size()) - 1);
}

template<typename T>
T ColaPrioridad<T>::pop() {
    if (estaVacia()) throw std::underflow_error("Cola vacía");
    T resultado = std::move(m_heap[0]);
    m_heap[0] = std::move(m_heap.back());
    m_heap.pop_back();
    if (!estaVacia()) bajarHeap(0);
    return resultado;
}

template<typename T>
const T& ColaPrioridad<T>::top() const {
    if (estaVacia()) throw std::underflow_error("Cola vacía");
    return m_heap[0];
}

template<typename T>
bool ColaPrioridad<T>::estaVacia() const { return m_heap.empty(); }

template<typename T>
int ColaPrioridad<T>::tamanio() const { return static_cast<int>(m_heap.size()); }

template<typename T>
void ColaPrioridad<T>::subirHeap(int idx) {
    while (idx > 0) {
        int padre = (idx - 1) / 2;
        if (m_cmp(m_heap[idx], m_heap[padre])) {
            std::swap(m_heap[idx], m_heap[padre]);
            idx = padre;
        } else break;
    }
}

template<typename T>
void ColaPrioridad<T>::bajarHeap(int idx) {
    int n = static_cast<int>(m_heap.size());
    while (true) {
        int menor = idx, izq = 2*idx+1, der = 2*idx+2;
        if (izq < n && m_cmp(m_heap[izq], m_heap[menor])) menor = izq;
        if (der < n && m_cmp(m_heap[der], m_heap[menor])) menor = der;
        if (menor == idx) break;
        std::swap(m_heap[idx], m_heap[menor]);
        idx = menor;
    }
}
