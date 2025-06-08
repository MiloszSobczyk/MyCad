#pragma once

#include <memory>
#include <vector>
#include <array>

#include "Point.h"
#include "Polyline.h"

enum class EdgeDirection
{
	Bottom,
	Right,
	Up,
	Left
};

class PatchEdge
{
private:
    std::array<std::shared_ptr<Point>, 4> controlPoints;

public:
    PatchEdge(const std::shared_ptr<Point>& p0,
        const std::shared_ptr<Point>& p1,
        const std::shared_ptr<Point>& p2,
        const std::shared_ptr<Point>& p3)
        : controlPoints{ p0, p1, p2, p3 }
    {
    }

    inline std::shared_ptr<Point> GetStart() const { return controlPoints[0]; }
    inline std::shared_ptr<Point> GetEnd()   const { return controlPoints[3]; }

	void Print()
	{
		std::cout << controlPoints[0]->GetId() << ' ' << controlPoints[1]->GetId() << ' '
			<< controlPoints[2]->GetId() << ' ' << controlPoints[3]->GetId() << '\n';
	}
};


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

	PatchEdge GetPatchEdge(EdgeDirection direction);
};