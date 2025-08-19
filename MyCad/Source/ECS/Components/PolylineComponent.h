#pragma once

#include <entt/entt.hpp>
#include <vector>


struct PolylineComponent
{
	std::vector<entt::entity> pointHandles;
};