#include "HistorialIncidentes.h"
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <ctime>

HistorialIncidentes::HistorialIncidentes(std::size_t capacidad)
    : m_capacidad(capacidad) {
    if (m_capacidad == 0) throw std::invalid_argument("La capacidad debe ser mayor que cero");
}

void HistorialIncidentes::registrar(int nodo, TipoIncidente tipo, int hora) {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_registros.push_back({nodo, tipo, hora, std::chrono::system_clock::now()});
    m_frecuenciaNodo[nodo]++;
    if (m_registros.size() > m_capacidad) {
        const int nodoAntiguo = m_registros.front().nodo;
        m_registros.erase(m_registros.begin());
        auto it = m_frecuenciaNodo.find(nodoAntiguo);
        if (it != m_frecuenciaNodo.end() && --it->second == 0)
            m_frecuenciaNodo.erase(it);
    }
}

int HistorialIncidentes::frecuenciaPorZona(int nodo) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    auto it = m_frecuenciaNodo.find(nodo);
    return (it != m_frecuenciaNodo.end()) ? it->second : 0;
}

int HistorialIncidentes::frecuenciaPorHora(int nodo, int hora) const {
    std::lock_guard<std::mutex> lock(m_mutex);
    int count = 0;
    for (const auto& r : m_registros)
        if (r.nodo == nodo && r.hora == hora) count++;
    return count;
}

std::vector<RegistroHistorial> HistorialIncidentes::todos() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_registros;
}

std::size_t HistorialIncidentes::capacidad() const { return m_capacidad; }

void HistorialIncidentes::limpiar() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_registros.clear();
    m_frecuenciaNodo.clear();
}

void HistorialIncidentes::exportarCSV(const std::string& ruta) const {
    const auto registros = todos();
    std::ofstream f(ruta);
    if (!f) throw std::runtime_error("No se pudo abrir el archivo CSV");
    f << "timestamp,nodo,tipo,hora\n";
    for (const auto& r : registros) {
        const auto tiempo = std::chrono::system_clock::to_time_t(r.timestamp);
        std::tm utc{};
#ifdef _WIN32
        gmtime_s(&utc, &tiempo);
#else
        gmtime_r(&tiempo, &utc);
#endif
        f << std::put_time(&utc, "%Y-%m-%dT%H:%M:%SZ") << ','
          << r.nodo << ',' << static_cast<int>(r.tipo) << ',' << r.hora << '\n';
    }
}
