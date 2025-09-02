#pragma once

#include <entt/entt.hpp>

#include "Algebra.h"

enum class CurveType
{
	BezierC0 = 0,
	BezierC2 = 1,
	Interpolating = 2,
};

struct CurveComponent
{
	int tessLevelU = 16;
	int tessLevelV = 16;

	CurveType curveType = CurveType::BezierC0;

	// perhaps some info if number of control points changed, so we know when to re-create bernstein polyline
	entt::entity controlPolylineHandle; // deBoor or interpolating
	entt::entity bernsteinPolylineHandle;

	std::function<std::vector<Algebra::Vector4>(CurveComponent&)> onUpdate;

	std::vector<Algebra::Vector4> CallUpdate()
	{
		if (onUpdate) return onUpdate(*this);
		return {};
	}
};