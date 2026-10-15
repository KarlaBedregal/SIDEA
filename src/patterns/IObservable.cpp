#include "IObservable.h"

void IObservable::suscribir(std::shared_ptr<IObserver> obs) {
    m_observadores.push_back(std::move(obs));
}

void IObservable::desuscribir(std::shared_ptr<IObserver> obs) {
    m_observadores.erase(
        std::remove(m_observadores.begin(), m_observadores.end(), obs),
        m_observadores.end()
    );
}

void IObservable::notificar(const std::string& evento) {
    for (auto& obs : m_observadores) {
        if (obs) obs->actualizar(evento);
    }
}
