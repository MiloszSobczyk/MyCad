#include "Patch.h"

Patch::Patch()
{
	bernsteinPolygon = std::make_shared<Polyline>();
	bernsteinPolygon->SetColor(ColorPalette::Get(Color::Teal));

    deBoorPolygon = std::make_shared<Polyline>();
    deBoorPolygon->SetColor(ColorPalette::Get(Color::Emerald));
}

void Patch::RenderBernsteinPolygon()
{
    if (bernsteinPolygon)
        bernsteinPolygon->Render();
}

void Patch::RenderDeBoorPolygon()
{
    if (deBoorPolygon)
        deBoorPolygon->Render();
}

void Patch::SwapBernsteinPoints(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint)
{
    for (int i = 0; i < bernsteinPoints.size(); ++i)
    {
        if (auto p = bernsteinPoints[i].lock())
        {
            if (p == oldPoint)
            {
                bernsteinPoints[i] = newPoint;
            }
        }
    }
    
    bernsteinPolygon->ReplacePoint(oldPoint, newPoint);
}

void Patch::SwapDeBoorPoints(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint)
{
    for (int i = 0; i < deBoorPoints.size(); ++i)
    {
        if (auto p = deBoorPoints[i].lock())
        {
            if (p == oldPoint)
            {
                deBoorPoints[i] = newPoint;
            }
        }
    }

    deBoorPolygon->ReplacePoint(oldPoint, newPoint);
}

void Patch::SetBernsteinPoints(std::vector<std::weak_ptr<Point>> bernsteinPoints)
{
    this->bernsteinPoints = bernsteinPoints;

    std::vector<std::weak_ptr<Point>> polygonPoints = {
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

	bernsteinPolygon->ClearPoints();
	for (const auto polygonPoint : polygonPoints)
	{
		bernsteinPolygon->AddPoint(polygonPoint);
	}
}

void Patch::SetDeBoorPoints(std::vector<std::weak_ptr<Point>> deBoorPoints)
{
    this->deBoorPoints = deBoorPoints;

    std::vector<std::weak_ptr<Point>> polygonPoints = {
        // Rows
        deBoorPoints[0],
        deBoorPoints[1],
        deBoorPoints[2],
        deBoorPoints[3],

        deBoorPoints[7],
        deBoorPoints[6],
        deBoorPoints[5],
        deBoorPoints[4],

        deBoorPoints[8],
        deBoorPoints[9],
        deBoorPoints[10],
        deBoorPoints[11],

        deBoorPoints[15],
        deBoorPoints[14],
        deBoorPoints[13],
        deBoorPoints[12],

        // Columns
        deBoorPoints[12],
        deBoorPoints[8],
        deBoorPoints[4],
        deBoorPoints[0],

        deBoorPoints[1],
        deBoorPoints[5],
        deBoorPoints[9],
        deBoorPoints[13],

        deBoorPoints[14],
        deBoorPoints[10],
        deBoorPoints[6],
        deBoorPoints[2],

        deBoorPoints[3],
        deBoorPoints[7],
        deBoorPoints[11],
        deBoorPoints[15]
    };

    deBoorPolygon->ClearPoints();
    for (const auto polygonPoint : polygonPoints)
    {
        deBoorPolygon->AddPoint(polygonPoint);
    }
}
