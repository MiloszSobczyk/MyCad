#pragma once

#include <memory>

class IObserver 
{
public:
    virtual ~IObserver() = default;

    virtual void OnNotified() = 0;
};
