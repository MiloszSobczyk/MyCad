#pragma once

#include <entt/entt.hpp>

// perhaps we could store functions here that notify components
struct NotificationComponent
{
	std::vector<entt::entity> toNotifys;

	void AddToNotify(entt::entity entity)
	{
		if (std::find(toNotifys.begin(), toNotifys.end(), entity) == toNotifys.end())
		{
			toNotifys.push_back(entity);
		}
	}

	void RemoveFromNotify(entt::entity entity)
	{
		auto it = std::remove(toNotifys.begin(), toNotifys.end(), entity);
		if (it != toNotifys.end())
		{
			toNotifys.erase(it, toNotifys.end());
		}
	}

	void ClearToNotify()
	{
		toNotifys.clear();
	}
};