#pragma once

#include <entt/entt.hpp>
#include <vector>

struct PatchComponent
{
	std::vector<entt::entity> pointHandles;

	entt::entity bernsteinPolylineHandle;
	entt::entity deBoorPolylineHandle;
};