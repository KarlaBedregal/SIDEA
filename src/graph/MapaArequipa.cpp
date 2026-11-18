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
    // Definición de las 20 zonas reales de Arequipa (nodos 0 al 19)
    // Coordenadas (x, y) proporcionales para visualización en lienzo ~800x600 px

    // 1. Centro Histórico y alrededores céntricos
    m_grafo.agregarNodo(0);
    m_posiciones[0] = {420.f, 290.f, "Centro Histórico"};

    m_grafo.agregarNodo(1);
    m_posiciones[1] = {490.f, 250.f, "Miraflores"};

    m_grafo.agregarNodo(2);
    m_posiciones[2] = {340.f, 190.f, "Cayma"};

    m_grafo.agregarNodo(3);
    m_posiciones[3] = {250.f, 140.f, "Cerro Colorado"};

    m_grafo.agregarNodo(4);
    m_posiciones[4] = {360.f, 250.f, "Yanahuara"};

    m_grafo.agregarNodo(5);
    m_posiciones[5] = {470.f, 170.f, "Alto Selva Alegre"};

    m_grafo.agregarNodo(6);
    m_posiciones[6] = {550.f, 350.f, "Paucarpata"};

    m_grafo.agregarNodo(7);
    m_posiciones[7] = {560.f, 240.f, "Mariano Melgar"};

    m_grafo.agregarNodo(8);
    m_posiciones[8] = {340.f, 400.f, "Jacobo Hunter"};

    m_grafo.agregarNodo(9);
    m_posiciones[9] = {460.f, 370.f, "José Luis Bustamante"};

    m_grafo.agregarNodo(10);
    m_posiciones[10] = {280.f, 330.f, "Sachaca"};

    m_grafo.agregarNodo(11);
    m_posiciones[11] = {200.f, 410.f, "Tiabaya"};

    // Nodos adicionales para completar las 20 zonas metropolitanas reales
    m_grafo.agregarNodo(12);
    m_posiciones[12] = {480.f, 470.f, "Socabaya"};

    m_grafo.agregarNodo(13);
    m_posiciones[13] = {620.f, 420.f, "Sabandía"};

    m_grafo.agregarNodo(14);
    m_posiciones[14] = {660.f, 500.f, "Characato"};

    m_grafo.agregarNodo(15);
    m_posiciones[15] = {710.f, 560.f, "Mollebaya"};

    m_grafo.agregarNodo(16);
    m_posiciones[16] = {110.f, 460.f, "Uchumayo"};

    m_grafo.agregarNodo(17);
    m_posiciones[17] = {150.f, 80.f, "Yura"};

    m_grafo.agregarNodo(18);
    m_posiciones[18] = {680.f, 230.f, "Chiguata"};

    m_grafo.agregarNodo(19);
    m_posiciones[19] = {540.f, 560.f, "Yarabamba"};

    // Conexiones viales principales (aristas bidireccionales con distancias proporcionales en km)
    // Conexiones desde Centro Histórico
    m_grafo.agregarAristaBidireccional(0, 4, 1.8, {"Puente Grau"});
    m_grafo.agregarAristaBidireccional(0, 1, 2.2, {"Av. Goyeneche"});
    m_grafo.agregarAristaBidireccional(0, 5, 3.2, {"Av. Arequipa"});
    m_grafo.agregarAristaBidireccional(0, 9, 2.5, {"Av. Dolores"});
    m_grafo.agregarAristaBidireccional(0, 6, 3.8, {"Av. Mariscal Castilla"});
    m_grafo.agregarAristaBidireccional(0, 8, 3.5, {"Av. Alfonso Ugarte"});
    m_grafo.agregarAristaBidireccional(0, 10, 3.4, {"Av. Fernandini"});

    // Sector Noroeste: Yanahuara, Cayma, Cerro Colorado, Yura
    m_grafo.agregarAristaBidireccional(4, 2, 1.9, {"Av. Ejército"});
    m_grafo.agregarAristaBidireccional(4, 10, 2.8, {"Av. Metropolitana"});
    m_grafo.agregarAristaBidireccional(2, 3, 3.5, {"Av. Aviación"});
    m_grafo.agregarAristaBidireccional(2, 5, 2.5, {"Puente Chilina"});
    m_grafo.agregarAristaBidireccional(3, 17, 7.5, {"Carretera a Yura"});
    m_grafo.agregarAristaBidireccional(3, 10, 5.5, {"Vía de Evitamiento"});
    m_grafo.agregarAristaBidireccional(3, 16, 9.0, {"Vía Evitamiento Oeste"});

    // Sector Este / Noreste: Alto Selva Alegre, Miraflores, Mariano Melgar, Chiguata
    m_grafo.agregarAristaBidireccional(5, 1, 2.3, {"Av. Las Torres"});
    m_grafo.agregarAristaBidireccional(1, 7, 2.0, {"Av. Sepúlveda"});
    m_grafo.agregarAristaBidireccional(7, 6, 2.6, {"Av. Simón Bolívar"});
    m_grafo.agregarAristaBidireccional(7, 18, 6.2, {"Carretera Arequipa-Chiguata"});

    // Sector Sureste: Paucarpata, José Luis Bustamante, Sabandía, Characato, Mollebaya
    m_grafo.agregarAristaBidireccional(6, 9, 2.4, {"Av. Los Incas"});
    m_grafo.agregarAristaBidireccional(6, 13, 3.0, {"Carretera Sabandía"});
    m_grafo.agregarAristaBidireccional(13, 18, 5.8, {"Vía Paisajista Este"});
    m_grafo.agregarAristaBidireccional(13, 14, 2.8, {"Av. Characato"});
    m_grafo.agregarAristaBidireccional(14, 15, 3.1, {"Carretera Mollebaya"});

    // Sector Sur: José Luis Bustamante, Socabaya, Yarabamba
    m_grafo.agregarAristaBidireccional(9, 12, 2.8, {"Av. Salaverry"});
    m_grafo.agregarAristaBidireccional(9, 8, 3.0, {"Av. Las Peñas"});
    m_grafo.agregarAristaBidireccional(12, 13, 3.5, {"Av. Paisajista"});
    m_grafo.agregarAristaBidireccional(12, 19, 4.5, {"Carretera Yarabamba"});
    m_grafo.agregarAristaBidireccional(15, 19, 4.8, {"Vía Quequeña-Yarabamba"});

    // Sector Suroeste: Jacobo Hunter, Sachaca, Tiabaya, Uchumayo
    m_grafo.agregarAristaBidireccional(8, 10, 2.2, {"Puente Huasacache"});
    m_grafo.agregarAristaBidireccional(8, 11, 3.8, {"Av. Huasacache-Tiabaya"});
    m_grafo.agregarAristaBidireccional(8, 12, 4.0, {"Av. San Jerónimo"});
    m_grafo.agregarAristaBidireccional(10, 11, 2.7, {"Av. Arancota"});
    m_grafo.agregarAristaBidireccional(10, 16, 5.2, {"Variante de Uchumayo"});
    m_grafo.agregarAristaBidireccional(11, 16, 3.6, {"Carretera Congata-Uchumayo"});
}
