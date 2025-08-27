#pragma once

#include <vector>
#include <entt/entt.hpp>

struct BezierSurfaceC0Component
{
	int tessLevelU = 16;
	int tessLevelV = 16;

	int widthPatches = 4;
	int heightPatches = 4;

	std::vector<entt::entity> patchHandles;
};