#pragma once

#include <memory>
#include <vector>

#include "Point.h"
#include "Polyline.h"

class Patch
{
private:
	std::vector<std::weak_ptr<Point>> bernsteinPoints;
	std::vector<std::weak_ptr<Point>> deBoorPoints;
	std::shared_ptr<Polyline> bernsteinPolygon;
	std::shared_ptr<Polyline> deBoorPolygon;

public:
	Patch();

	void RenderBernsteinPolygon();
	void RenderDeBoorPolygon();

	void SwapBernsteinPoints(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint);
	void SwapDeBoorPoints(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint);

	void SetBernsteinPoints(std::vector<std::weak_ptr<Point>> bernsteinPoints);
	inline std::vector<std::weak_ptr<Point>> GetBernsteinPoints() { return bernsteinPoints; };

	void SetDeBoorPoints(std::vector<std::weak_ptr<Point>> deBoorPoints);
	inline std::vector<std::weak_ptr<Point>> GetDeBoorPoints() { return deBoorPoints; };
};