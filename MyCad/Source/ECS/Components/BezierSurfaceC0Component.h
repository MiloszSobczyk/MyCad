#pragma once

#include <vector>
#include <entt/entt.hpp>

struct BezierSurfaceC0Component
{
	int tessLevelU = 16;
	int tessLevelV = 16;

	int widthPatches = 4;
	int heightPatches = 4;

	std::vector<entt::entity> pointHandles;
	std::vector<entt::entity> patchHandles;

	inline int GetColumns() const { return widthPatches * 3 + 1; }
	inline int GetRows() const { return heightPatches * 3 + 1; }
};