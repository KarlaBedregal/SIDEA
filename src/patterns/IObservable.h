#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include "IObserver.h"

class IObservable {
public:
    virtual ~IObservable() = default;

    void suscribir(std::shared_ptr<IObserver> obs);
    void desuscribir(std::shared_ptr<IObserver> obs);

protected:
    void notificar(const std::string& evento);

private:
    std::vector<std::shared_ptr<IObserver>> m_observadores;
};
