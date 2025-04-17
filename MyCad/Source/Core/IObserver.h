#pragma once

#include <memory>

class IObserver 
{
public:
    virtual ~IObserver() = default;

    virtual void OnNotified(std::shared_ptr<void> sender) = 0;
};
