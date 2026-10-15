#pragma once
#include <string>

class IObserver {
public:
    virtual ~IObserver() = default;
    virtual void actualizar(const std::string& evento) = 0;
};
