#pragma once
#include <algorithm>
#include <memory>
#include <mutex>
#include <vector>
#include "IObserver.h"

class IObservable {
public:
    virtual ~IObservable() = default;

    void suscribir(std::shared_ptr<IObserver> obs);
    void desuscribir(std::shared_ptr<IObserver> obs);

protected:
    void notificar(const std::string& evento);

private:
    mutable std::mutex m_mutexObs;
    std::vector<std::shared_ptr<IObserver>> m_observadores;
};
