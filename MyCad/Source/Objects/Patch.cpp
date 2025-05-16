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
