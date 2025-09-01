#pragma once

#include <entt/entt.hpp>

struct GregoryPatch
{
	std::array<entt::entity, 3> surfaceHandles;
	std::array<entt::entity, 3> edgeHandles;
};