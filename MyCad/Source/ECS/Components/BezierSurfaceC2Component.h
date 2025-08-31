#pragma once

enum class ConnectionType2
{
	Flat = 0,
	Columns = 1,
	Rows = 2,
};

struct BezierSurfaceC2Component
{
	bool initialized = false;

	int tessLevelU = 16;
	int tessLevelV = 16;

	int widthPatches = 4;
	int heightPatches = 4;

	ConnectionType2 connectionType = ConnectionType2::Flat;

	std::vector<entt::entity> pointHandles;
	std::vector<entt::entity> patchHandles;

	inline int GetColumns() const { return widthPatches + 3; }
	inline int GetRows() const { return heightPatches + 3; }
};