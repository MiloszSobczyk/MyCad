#pragma once

#include "Algebra.h"

struct RotationComponent 
{
	Algebra::Quaternion rotation = Algebra::Quaternion(1.f, 0.f, 0.f, 0.f);
};
