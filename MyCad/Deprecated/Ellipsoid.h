#pragma once
#include <vector>
#include <Utils/Shader.h>
#include "Algebra.h"

using namespace Algebra;


class Ellipsoid
{
private:
	float a, b, c;

	Matrix4 tempInverseRotations;
	Matrix4 inverseRotations;
	Matrix4 tempFinalMatrix;
	Matrix4 finalMatrix;
public:
	Ellipsoid(float a, float b, float c);

	Matrix4 CalculateInverseTransformations();
	void SetupRotations();

	std::pair<bool, float> CalculatePoint(float x, float y);
	Vector4 CalculateGradient(float x, float y, float z);

	void Refresh();
};