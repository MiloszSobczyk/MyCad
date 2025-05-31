#pragma once

#include <memory>
#include <vector>

#include "Point.h"
#include "Polyline.h"

class Patch
{
private:
	std::vector<std::weak_ptr<Point>> points;
	std::shared_ptr<Polyline> polyline;

public:
	bool drawPolyline = true;

	Patch(std::vector<std::weak_ptr<Point>>& points);

	void Render();
	void SetBernsteinPoints(std::vector<std::weak_ptr<Point>> bernsteinPoints);

	inline std::vector<std::weak_ptr<Point>> GetPoints() { return points; };
};