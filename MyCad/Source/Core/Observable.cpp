#include "Observable.h"

void Observable::NotifyObservers()
{
	for (auto it = observers.begin(); it != observers.end(); )
	{
		if (auto observer = it->lock())
		{
			observer->OnNotified();
			++it;
		}
		else
		{
			it = observers.erase(it);
		}
	}
}

void Observable::AddObserver(std::shared_ptr<IObserver> observer)
{
	auto it = std::find_if(observers.begin(), observers.end(),
		[&](const std::weak_ptr<IObserver>& o) {
			return o.lock() == observer;
		});

	if (it == observers.end()) 
	{
		observers.push_back(observer);
	}
}

void Observable::RemoveObserver(std::shared_ptr<IObserver> observer)
{
	observers.erase(
		std::remove_if(observers.begin(), observers.end(),
			[&](const std::weak_ptr<IObserver>& o) {
				return o.lock() == observer;
			}),
		observers.end()
	);
}

void Observable::ClearObservers()
{
	observers.clear();
}
