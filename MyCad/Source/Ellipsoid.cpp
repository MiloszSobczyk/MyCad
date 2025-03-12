#pragma once

#include "Ellipsoid.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "UserInterface.h"

Ellipsoid::Ellipsoid(float a, float b, float c)
	: a(a), b(b), c(c), inverseRotations(Vector4(1.f, 1.f, 1.f, 1.f)), 
		tempInverseRotations(Vector4(1.f, 1.f, 1.f, 1.f))
{
}

void Ellipsoid::Refresh()
{
	UIValues values = UserInterface::values;

	Matrix4 diag(Vector4(1 / values.a / values.a, 1 / values.b / values.b, 1 / values.c / values.c, -1.f));
	Matrix4 inv = CalculateInverseTransformations();
	finalMatrix = inv.Transpose() * diag * inv;
}

Matrix4 Ellipsoid::CalculateInverseTransformations()
{
	UIValues values = UserInterface::values;

	Matrix4 inverseTranslation = Matrix4::Translation(
		-values.translationX, 
		-values.translationY, 
		-values.translationZ
	);

	Matrix4 inverseScaling = Matrix4(Vector4(1 / values.scale, 1 / values.scale, 1 / values.scale, 1));

	SetupRotations();

	if (values.moving)
	{
		return inverseScaling * tempInverseRotations * inverseTranslation;
	}
	else
	{
		return inverseScaling * inverseRotations * inverseTranslation;
	}
}

void Ellipsoid::SetupRotations()
{
	UIValues values = UserInterface::values;

	if (values.moving)
		tempInverseRotations = inverseRotations * values.rotation;
	else
	{
		inverseRotations = inverseRotations * values.rotation;
		UserInterface::values.rotation = Matrix4::Identity();
	}
}

std::pair<bool, float> Ellipsoid::CalculatePoint(float x, float y)
{
	float aZ = finalMatrix[2][2];
	// possible easier calculation of bZ
	float bZ = ((finalMatrix[0][2] + finalMatrix[2][0]) * x + (finalMatrix[1][2] + finalMatrix[2][1]) * y +
		finalMatrix[2][3] + finalMatrix[3][2]);
	float cZ = Vector4(x, y, 0.f, 1.f) * finalMatrix * Vector4(x, y, 0.f, 1.f);

	float delta = bZ * bZ - 4 * aZ * cZ;

	if (delta < 0)
	{
		return std::make_pair(false, 0.f);
	}

	float deltaSquared = sqrtf(delta);
	float z1 = (-bZ + deltaSquared) / (2.f * aZ);
	float z2 = (-bZ - deltaSquared) / (2.f * aZ);

	return std::make_pair(true, std::max(z1, z2));
}

Vector4 Ellipsoid::CalculateGradient(float x, float y, float z)
{
	float a = 2 * finalMatrix[0][0];
	float b = Vector4(0, y, z, 1) * finalMatrix * Vector4(1, 0, 0, 0) 
		+ Vector4(1, 0, 0, 0) * finalMatrix * Vector4(0, y, z, 1);
	float xGrad = a * x + b;

	a = 2 * finalMatrix[1][1];
	b = Vector4(x, 0, z, 1) * finalMatrix * Vector4(0, 1, 0, 0)
		+ Vector4(0, 1, 0, 0) * finalMatrix * Vector4(x, 0, z, 1);
	float yGrad = a * y + b;

	a = 2 * finalMatrix[2][2];
	b = Vector4(x, y, 0, 1) * finalMatrix * Vector4(0, 0, 1, 0)
		+ Vector4(0, 0, 1, 0) * finalMatrix * Vector4(x, y, 0, 1);
	float zGrad = a * z + b;

	return Vector4(xGrad, yGrad, zGrad, 0.f);
}


