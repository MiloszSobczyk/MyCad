#pragma once

#include <vector>

#include "Algebra.h"
#include "Engine/VertexLayout.h"

using namespace Algebra;

class Torus
{
private:
	Vector4 center;

	float majorR;
	float minorR;
	int majorDivisions;
	int minorDivisions;

	std::vector<PositionVertexData> vertices;
	std::vector<unsigned int> indices;

public:
	Torus(float majorR, float minorR, int majorDivisions, int minorDivisions);
	
	void CalculatePoints();
};