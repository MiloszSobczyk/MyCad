#include "Patch.h"

Patch::Patch(std::vector<std::weak_ptr<Point>>& points)
	: points(points)
{
	polyline = std::make_shared<Polyline>();
	polyline->SetColor(ColorPalette::Get(Color::Teal));
}

void Patch::Render()
{
	if (polyline)
		polyline->Render();
}

void Patch::SetBernsteinPoints(std::vector<std::weak_ptr<Point>> bernsteinPoints)
{
    std::vector<std::weak_ptr<Point>> polyPoints = {
        // Rows
        bernsteinPoints[0],
        bernsteinPoints[1],
        bernsteinPoints[2],
        bernsteinPoints[3],

        bernsteinPoints[7],
        bernsteinPoints[6],
        bernsteinPoints[5],
        bernsteinPoints[4],

        bernsteinPoints[8],
        bernsteinPoints[9],
        bernsteinPoints[10],
        bernsteinPoints[11],

        bernsteinPoints[15],
        bernsteinPoints[14],
        bernsteinPoints[13],
        bernsteinPoints[12],

        // Columns
        bernsteinPoints[12],
        bernsteinPoints[8],
        bernsteinPoints[4],
        bernsteinPoints[0],

        bernsteinPoints[1],
        bernsteinPoints[5],
        bernsteinPoints[9],
        bernsteinPoints[13],

        bernsteinPoints[14],
        bernsteinPoints[10],
        bernsteinPoints[6],
        bernsteinPoints[2],

        bernsteinPoints[3],
        bernsteinPoints[7],
        bernsteinPoints[11],
        bernsteinPoints[15]
    };

	polyline->ClearPoints();
	for (const auto polyPoint : polyPoints)
	{
		polyline->AddPoint(polyPoint);
	}
}
