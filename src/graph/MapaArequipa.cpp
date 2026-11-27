#include "MapaArequipa.h"

MapaArequipa::MapaArequipa() { construir(); }

const Grafo<DatosVia>& MapaArequipa::grafo() const { return m_grafo; }
int MapaArequipa::totalNodos() const { return m_grafo.totalNodos(); }

PosicionNodo MapaArequipa::posicion(int nodo) const {
    auto it = m_posiciones.find(nodo);
    if (it == m_posiciones.end()) return {0.f, 0.f, "?", "", 0.0, 0.0};
    return it->second;
}

std::string MapaArequipa::nombreZona(int nodo) const {
    return posicion(nodo).zona;
}

std::string MapaArequipa::direccion(int nodo) const {
    return posicion(nodo).direccion;
}

void MapaArequipa::construir() {
    // 20 zonas reales de Arequipa (nodos 0 al 19) proyectadas sobre
    // assets/mapa_arequipa.png (1280 x 1536 px, bbox Web Mercator de mapa_bbox.txt)
    // con su dirección exacta de referencia y coordenadas WGS84.

    m_grafo.agregarNodo(0);
    m_posiciones[0] = {803.40f, 1118.20f, "Centro Histórico",
                       "Plaza de Armas, Portal de la Municipalidad s/n, Cercado",
                       -16.3989, -71.5369};

    m_grafo.agregarNodo(1);
    m_posiciones[1] = {994.40f, 1013.10f, "Miraflores",
                       "Av. Goyeneche cdra. 18 / Plaza Mayta Cápac, Miraflores",
                       -16.3816, -71.5041};

    m_grafo.agregarNodo(2);
    m_posiciones[2] = {748.00f, 1016.80f, "Cayma",
                       "Av. Cayma cdra. 8 / Plaza Principal de Cayma",
                       -16.3822, -71.5464};

    m_grafo.agregarNodo(3);
    m_posiciones[3] = {667.00f, 984.00f, "Cerro Colorado",
                       "Av. Aviación km 5 / Plaza Las Américas, Cerro Colorado",
                       -16.3768, -71.5603};

    m_grafo.agregarNodo(4);
    m_posiciones[4] = {702.60f, 1096.30f, "Yanahuara",
                       "Calle Miguel Grau cdra. 2 / Mirador de Yanahuara",
                       -16.3953, -71.5542};

    m_grafo.agregarNodo(5);
    m_posiciones[5] = {898.30f, 996.70f, "Alto Selva Alegre",
                       "Av. Arequipa cdra. 6 / Plaza Espíritu Santo, A.S.A.",
                       -16.3789, -71.5206};

    m_grafo.agregarNodo(6);
    m_posiciones[6] = {1184.30f, 1218.40f, "Paucarpata",
                       "Av. Jesús cdra. 12 / Plaza Principal de Paucarpata",
                       -16.4154, -71.4715};

    m_grafo.agregarNodo(7);
    m_posiciones[7] = {1116.80f, 1124.90f, "Mariano Melgar",
                       "Av. Simón Bolívar cdra. 4 / Plaza Umachiri, M. Melgar",
                       -16.4000, -71.4831};

    m_grafo.agregarNodo(8);
    m_posiciones[8] = {698.50f, 1415.20f, "Jacobo Hunter",
                       "Av. Viña del Mar cdra. 3 / Plaza Principal de Hunter",
                       -16.4478, -71.5549};

    m_grafo.agregarNodo(9);
    m_posiciones[9] = {879.10f, 1286.40f, "José Luis Bustamante",
                       "Av. Dolores cdra. 4 / Parque de la Amistad, J.L.B. y R.",
                       -16.4266, -71.5239};

    m_grafo.agregarNodo(10);
    m_posiciones[10] = {527.20f, 1245.70f, "Sachaca",
                        "Av. Fernandini cdra. 5 / Plaza Principal de Sachaca",
                        -16.4199, -71.5843};

    m_grafo.agregarNodo(11);
    m_posiciones[11] = {406.60f, 1343.50f, "Tiabaya",
                        "Av. Arequipa cdra. 3 / Plaza Principal de Tiabaya",
                        -16.4360, -71.6050};

    m_grafo.agregarNodo(12);
    m_posiciones[12] = {838.30f, 1525.70f, "Socabaya",
                        "Av. Salaverry cdra. 8 / Plaza San Martín de Socabaya",
                        -16.4660, -71.5309};

    m_grafo.agregarNodo(13);
    m_posiciones[13] = {1049.20f, 1470.40f, "Sabandía",
                        "Av. Principal cdra. 2 / Plaza de Armas de Sabandía",
                        -16.4569, -71.4947};

    m_grafo.agregarNodo(14);
    m_posiciones[14] = {1059.70f, 1692.20f, "Characato",
                        "Av. Characato cdra. 4 / Plaza Principal de Characato",
                        -16.4934, -71.4929};

    m_grafo.agregarNodo(15);
    m_posiciones[15] = {1309.60f, 1629.60f, "Mollebaya",
                        "Calle Principal s/n / Plaza de Armas de Mollebaya",
                        -16.4831, -71.4500};

    m_grafo.agregarNodo(16);
    m_posiciones[16] = {51.29f, 1307.04f, "Uchumayo",
                        "Variante de Uchumayo km 12 / Estación de Uchumayo",
                        -16.4300, -71.6660};

    m_grafo.agregarNodo(17);
    m_posiciones[17] = {132.85f, 329.62f, "Yura",
                        "Carretera a Yura km 26 / Ciudad de Dios, Yura",
                        -16.2690, -71.6520};

    m_grafo.agregarNodo(18);
    m_posiciones[18] = {1648.00f, 1139.40f, "Chiguata",
                        "Carretera Arequipa–Chiguata km 18 / Plaza de Chiguata",
                        -16.4024, -71.3919};

    m_grafo.agregarNodo(19);
    m_posiciones[19] = {1152.90f, 2017.20f, "Yarabamba",
                        "Carretera a Yarabamba km 14 / Plaza de Yarabamba",
                        -16.5469, -71.4769};

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
