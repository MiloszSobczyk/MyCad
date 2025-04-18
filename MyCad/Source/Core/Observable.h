#pragma once

#include <vector>
#include <memory>
#include <algorithm>

#include "IObserver.h"

class Observable
{
private:
    std::vector<std::weak_ptr<IObserver>> observers;

protected:
    void NotifyObservers();

public:
    void AddObserver(std::shared_ptr<IObserver> observer);
    void RemoveObserver(std::shared_ptr<IObserver> observer);
};