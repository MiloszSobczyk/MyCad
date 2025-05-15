#include "Patch.h"

Patch::Patch(std::vector<std::weak_ptr<Point>>& points, std::vector<std::size_t>& patchUniqueIndices)
	: points(points), patchUniqueIndices(patchUniqueIndices)
{
}
