#pragma once

#include <memory>
#include <vector>

#include "Point.h"

class Patch
{
private:
	std::vector<std::weak_ptr<Point>> points;
	std::vector<std::size_t> uniqueIndices;

public:
	Patch(std::vector<std::weak_ptr<Point>>& points, std::vector<std::size_t>& uniqueIndices);

	void SetColor(Algebra::Vector4 color);

	inline std::vector<std::weak_ptr<Point>> GetPoints() { return points; }
};