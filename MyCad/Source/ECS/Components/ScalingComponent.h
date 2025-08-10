#pragma once

#include "Algebra.h"

struct ScalingComponent
{
	Algebra::Vector4 scaling = Algebra::Vector4(1.f, 1.f, 1.f);

	void AddScaling(const Algebra::Vector4& newScaling)
	{
		scaling.x *= newScaling.x;
		scaling.y *= newScaling.y;
		scaling.z *= newScaling.z;
		scaling.w = 1.0f;
	}
};