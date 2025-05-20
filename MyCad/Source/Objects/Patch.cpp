#include "Patch.h"

Patch::Patch(std::vector<std::weak_ptr<Point>>& points, std::vector<std::size_t>& uniqueIndices)
	: points(points), uniqueIndices(uniqueIndices)
{
}

void Patch::SetColor(Algebra::Vector4 color)
{
	for (auto point : points)
	{
		if (auto sp = point.lock())
		{
			sp->SetColor(color);
		}
	}
}

void Patch::Render()
{
	if(polyline)
		polyline->Render();
}

void Patch::AddBernstein(std::vector<std::weak_ptr<Point>> points)
{
	std::vector<std::weak_ptr<Point>> points2;

	points2.push_back(points[0]);
	points2.push_back(points[1]);
	points2.push_back(points[2]);
	points2.push_back(points[3]);

	points2.push_back(points[7]);
	points2.push_back(points[6]);
	points2.push_back(points[5]);
	points2.push_back(points[4]);

	points2.push_back(points[8]);
	points2.push_back(points[9]);
	points2.push_back(points[10]);
	points2.push_back(points[11]);

	points2.push_back(points[15]);
	points2.push_back(points[14]);
	points2.push_back(points[13]);
	points2.push_back(points[12]);

	points2.push_back(points[12]);
	points2.push_back(points[8]);
	points2.push_back(points[4]);
	points2.push_back(points[0]);

	points2.push_back(points[1]);
	points2.push_back(points[5]);
	points2.push_back(points[9]);
	points2.push_back(points[13]);

	points2.push_back(points[14]);
	points2.push_back(points[10]);
	points2.push_back(points[6]);
	points2.push_back(points[2]);

	points2.push_back(points[3]);
	points2.push_back(points[7]);
	points2.push_back(points[11]);
	points2.push_back(points[15]);

	polyline = std::make_shared<Polyline>(points2);
	polyline->InitFromPoints();
	polyline->SetColor(ColorPalette::Get(Color::Teal));
}
