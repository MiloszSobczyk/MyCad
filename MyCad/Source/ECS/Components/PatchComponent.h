#pragma once

#include <entt/entt.hpp>
#include <vector>
#include <functional>

#include "Algebra.h"

struct PatchComponent
{
	std::vector<entt::entity> pointHandles;

	entt::entity bernsteinPolylineHandle;
	entt::entity deBoorPolylineHandle;

	std::function<std::vector<Algebra::Vector4>(PatchComponent&)> onUpdate;

	std::vector<Algebra::Vector4> CallUpdate() 
	{
		if (onUpdate) return onUpdate(*this);
		return {};
	}
};