#pragma once
#include <vector>
#include <Deprecated/Ellipsoid.h>
#include <Core/Window.h>
#include "Algebra.h"

using namespace Algebra;

struct RayCasterPoint
{
	Vector4 point;
	Vector4 color;
};

class RayCaster
{
private:
	std::vector<RayCasterPoint> points;
	unsigned int VAO;
	unsigned int VBO;
	Shader shader;

public:
	RayCaster();
	void CalculatePoints(Ellipsoid& shape, Window& window, int raySize);
	void Render();
	void Save();
};