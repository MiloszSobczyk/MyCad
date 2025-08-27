#pragma once

#include <entt/entt.hpp>

struct InterpolatingCurveComponent
{
	int tessLevelU = 16;
	int tessLevelV = 16;

	entt::entity bernsteinPolylineHandle;
	entt::entity interpolatingPolylineHandle;
};