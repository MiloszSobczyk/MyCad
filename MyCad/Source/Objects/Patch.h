#pragma once

#include <memory>
#include <vector>

#include "Point.h"
#include "Polyline.h"

class Patch
{
private:
	std::vector<std::weak_ptr<Point>> points;
	std::vector<std::size_t> uniqueIndices;
	std::shared_ptr<Polyline> polyline;

public:
	bool drawPolyline = true;

	Patch(std::vector<std::weak_ptr<Point>>& points, std::vector<std::size_t>& uniqueIndices);

	void SetColor(Algebra::Vector4 color);

	void Render();

	void AddBernstein(std::vector<std::weak_ptr<Point>> points);

	inline std::vector<std::weak_ptr<Point>> GetPoints() { return points; };
};