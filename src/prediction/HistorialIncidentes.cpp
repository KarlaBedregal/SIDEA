#include "HistorialIncidentes.h"
#include <fstream>

void HistorialIncidentes::registrar(int nodo, TipoIncidente tipo, int hora) {
    m_registros.push_back({nodo, tipo, hora});
    m_frecuenciaNodo[nodo]++;
}

int HistorialIncidentes::frecuenciaPorZona(int nodo) const {
    auto it = m_frecuenciaNodo.find(nodo);
    return (it != m_frecuenciaNodo.end()) ? it->second : 0;
}

int HistorialIncidentes::frecuenciaPorHora(int nodo, int hora) const {
    int count = 0;
    for (auto& r : m_registros) {
        if (r.nodo == nodo && r.hora == hora) count++;
    }
    return count;
}

std::vector<RegistroHistorial> HistorialIncidentes::todos() const { return m_registros; }

void HistorialIncidentes::exportarCSV(const std::string& ruta) const {
    std::ofstream f(ruta);
    f << "nodo,tipo,hora\n";
    for (auto& r : m_registros) {
        f << r.nodo << "," << static_cast<int>(r.tipo) << "," << r.hora << "\n";
    }
}
