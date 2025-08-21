#pragma once

#include <entt/entt.hpp>

struct BezierCurveC2Component
{
	int tessLevelU = 16;
	int tessLevelV = 16;

	entt::entity deBoorPolylineHandle;
	entt::entity bernsteinPolylineHandle;
};