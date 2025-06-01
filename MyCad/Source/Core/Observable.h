#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "IObserver.h"

class Observable
{
protected:
    std::vector<std::weak_ptr<IObserver>> observers;
    
    void NotifyObservers();

public:
    void AddObserver(std::shared_ptr<IObserver> observer);
    void RemoveObserver(std::shared_ptr<IObserver> observer);
};