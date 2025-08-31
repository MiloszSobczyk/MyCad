#pragma once

#include <entt/entt.hpp>

enum class ConnectionType
{
	Flat = 0,
	Columns = 1,
	Rows = 2,
};

struct BezierSurfaceComponent
{
	bool initialized = false;

	int tessLevelU = 16;
	int tessLevelV = 16;

	int widthPatches = 4;
	int heightPatches = 4;

	ConnectionType connectionType = ConnectionType::Flat;

	std::vector<entt::entity> pointHandles;
	std::vector<entt::entity> patchHandles;

	inline int GetColumns() const { return widthPatches * 3 + 1; }
	inline int GetRows() const { return heightPatches * 3 + 1; }
};