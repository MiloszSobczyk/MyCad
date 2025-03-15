#pragma once

#include "Algebra.h"
#include "Utils/Logger.h"

using namespace Algebra;

struct PositionVertexData
{
	Vector4 Position;
};

struct PositionColorVertexData
{
	Vector4 Position;
	Vector4 Color;
};

struct PositionNormalVertexData
{
	Vector4 Position;
	Vector4 Normal;
};

enum VertexDataType
{
	Position,
	PositionColor,
	PositionNormal,
};

class VertexLayout
{
private:
	VertexDataType dataType;
public:
	VertexLayout(VertexDataType dataType);
	void EnableAttribArrays() const;
};