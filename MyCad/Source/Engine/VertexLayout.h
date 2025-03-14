#pragma once

#include "Algebra.h"
#include "Utils/Logger.h"

using namespace Algebra;

struct PositionColorVertex
{
	Vector4 point;
	Vector4 color;
};

class VertexLayout
{
public:
	void virtual EnableAttribArrays() const;
};