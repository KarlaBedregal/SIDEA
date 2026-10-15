#include "MapaArequipa.h"

MapaArequipa::MapaArequipa() { construir(); }

const Grafo<DatosVia>& MapaArequipa::grafo() const { return m_grafo; }
int MapaArequipa::totalNodos() const { return m_grafo.totalNodos(); }

PosicionNodo MapaArequipa::posicion(int nodo) const {
    auto it = m_posiciones.find(nodo);
    if (it == m_posiciones.end()) return {0.f, 0.f, "?"};
    return it->second;
}

std::string MapaArequipa::nombreZona(int nodo) const {
    return posicion(nodo).zona;
}

void MapaArequipa::construir() {
    // TODO Sprint 1 (B4): definir ~25 nodos representando zonas de Arequipa
    // Ejemplo de estructura esperada:
    //   0 = Centro Histórico, 1 = Miraflores, 2 = Cayma, 3 = Cerro Colorado, ...
    //
    // m_grafo.agregarNodo(0);
    // m_posiciones[0] = {400.f, 300.f, "Centro Histórico"};
    // m_grafo.agregarAristaBidireccional(0, 1, 2.5, {"Av. Ejército"});
}
