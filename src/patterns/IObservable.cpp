#include "IObservable.h"

void IObservable::suscribir(std::shared_ptr<IObserver> obs) {
    std::lock_guard<std::mutex> lock(m_mutexObs);
    m_observadores.push_back(std::move(obs));
}

void IObservable::desuscribir(std::shared_ptr<IObserver> obs) {
    std::lock_guard<std::mutex> lock(m_mutexObs);
    m_observadores.erase(
        std::remove(m_observadores.begin(), m_observadores.end(), obs),
        m_observadores.end()
    );
}

void IObservable::notificar(const std::string& evento) {
    // Copia local para no mantener el lock durante los callbacks
    std::vector<std::shared_ptr<IObserver>> copia;
    {
        std::lock_guard<std::mutex> lock(m_mutexObs);
        copia = m_observadores;
    }
    for (auto& obs : copia) {
        if (obs) obs->actualizar(evento);
    }
}
