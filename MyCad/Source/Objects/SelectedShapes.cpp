#include "SelectedShapes.h"

#include "Objects/Shape.h"
#include "Objects/Point.h"
#include "Objects/BezierSurfaceC0.h"
#include <unordered_set>

void SelectedShapes::Clear()
{
	selectedShapes.clear();
}

void SelectedShapes::AddShape(std::shared_ptr<Shape> shape)
{
	if (IsSelected(shape))
	{
		return;
	}

	selectedShapes.push_back(shape);
}

void SelectedShapes::RemoveShape(std::shared_ptr<Shape> shape)
{
	selectedShapes.erase(std::find(selectedShapes.begin(), selectedShapes.end(), shape));
}

void SelectedShapes::ToggleShape(std::shared_ptr<Shape> shape)
{
	if (IsSelected(shape))
	{
		RemoveShape(shape);
	}
	else
	{
		AddShape(shape);
	}
}

bool SelectedShapes::IsSelected(std::shared_ptr<Shape> shape) const
{
	return std::find(selectedShapes.begin(), selectedShapes.end(), shape) != selectedShapes.end();
}

std::shared_ptr<Shape> SelectedShapes::GetAt(int index) const
{
	if (index >= 0 && index < static_cast<int>(selectedShapes.size()))
	{
		return selectedShapes[index];
	}
	return nullptr;
}

std::optional<Algebra::Vector4> SelectedShapes::GetAveragePosition() const
{
	Algebra::Vector4 result;
	auto castShapes = GetSelectedWithType<ITranslation>();
	if (castShapes.empty())
	{
		return std::nullopt;
	}

	for (auto& shape : castShapes)
	{
		result += shape->GetTranslationComponent()->GetTranslation();
	}

	result = result / castShapes.size();
	return result;
}

std::optional<std::shared_ptr<Point>> SelectedShapes::MergePoints()
{
	const auto points = GetSelectedWithType<Point>();

	if (points.size() <= 1)
	{
		return std::nullopt;
	}

	Algebra::Vector4 newPosition;
	for (const auto& point : points) 
	{
		selectedShapes.erase(std::find(selectedShapes.begin(), selectedShapes.end(), point));

		newPosition += point->GetTranslationComponent()->GetTranslation();
	}

	newPosition = newPosition / points.size();

	auto newPoint = std::make_shared<Point>();
	newPoint->GetTranslationComponent()->SetTranslation(newPosition);
	selectedShapes.push_back(newPoint);

	for (const auto& point : points)
	{
		point->SwapFor(newPoint);
	}

	return newPoint;
}

// TODO: remove enum and use directional graph, so edges are connected only if first's end is second's start 
// and not the other way around
enum class EdgeConnectionType
{
	None = 0,
	EndStart = 1,
	StartEnd = 2,
};

static EdgeConnectionType SharesEndpoint(const PatchEdge& e1, const PatchEdge& e2)
{
	auto s1 = e1.GetStart().get();
	auto e1p = e1.GetEnd().get();
	auto s2 = e2.GetStart().get();
	auto e2p = e2.GetEnd().get();
	if (e1p == s2)
	{
		return EdgeConnectionType::EndStart;
	}
	else if (s1 == e2p)
	{
		return EdgeConnectionType::StartEnd;
	}
	return EdgeConnectionType::None;
}

std::vector<std::array<PatchEdge, 3>> SelectedShapes::FindEdgeCycles()
{
	auto bezierSurfaces = GetSelectedWithType<BezierSurfaceC0>();

	if (bezierSurfaces.empty())
	{
		return {};
	}

	// Populate graph
	std::vector<PatchEdge> edgeVertices;
	for (auto& surface : bezierSurfaces)
	{
		auto edges = surface->GetOutsideEdges();
		edgeVertices.insert(edgeVertices.end(), edges.begin(), edges.end());
	}

	for (int i = 0; i < edgeVertices.size(); ++i)
	{
		if (i % 4 == 0)
			std::cout << '\n';

		std::cout << i << "   ";
		edgeVertices[i].Print();
	}
	std::cout << '\n';

	int N = edgeVertices.size();
	std::vector<std::vector<EdgeConnectionType>> edgeEdges(N, std::vector<EdgeConnectionType>(N, EdgeConnectionType::None));

	for (int i = 0; i < N; ++i)
	{
		for (int j = 0; j < N; ++j)
		{
			edgeEdges[i][j] = SharesEndpoint(edgeVertices[i], edgeVertices[j]);
		}
	}

	for (int i = 0; i < edgeEdges.size(); ++i)
	{
		for (int j = 0; j < edgeEdges[0].size(); ++j)
		{
			std::cout << static_cast<int>(edgeEdges[i][j]) << "  ";
		}
		std::cout << '\n';
	}

	// Find cycles
	std::vector<std::array<int, 3>> indexCycles;

	for (int i = 0; i < edgeVertices.size(); ++i)
	{
		for (int j = 0; j < edgeVertices.size(); ++j)
		{
			if (j == i) continue;

			for (int k = 0; k < edgeVertices.size(); ++k)
			{
				if (k == i || k == j) continue;

				if (edgeEdges[i][j] == EdgeConnectionType::EndStart && 
					edgeEdges[j][k] == EdgeConnectionType::EndStart && 
					edgeEdges[k][i] == EdgeConnectionType::EndStart)
				{
					indexCycles.push_back({ i, j, k });
				}
				else if (edgeEdges[i][j] == EdgeConnectionType::StartEnd &&
					edgeEdges[j][k] == EdgeConnectionType::StartEnd &&
					edgeEdges[k][i] == EdgeConnectionType::StartEnd)
				{
					indexCycles.push_back({ i, j, k });
				}
			}
		}
	}

	// Remove cycle permutations
	std::vector<std::array<int, 3>> uniqueCycles;
	uniqueCycles.reserve(indexCycles.size());

	auto make_key = [](const std::array<int, 3>& a) {
		return std::to_string(a[0]) + "_" +
			std::to_string(a[1]) + "_" +
			std::to_string(a[2]);
		};

	std::unordered_set<std::string> seen;
	seen.reserve(indexCycles.size());

	for (auto const& cycle : indexCycles) 
	{
		std::array<int, 3> sorted = cycle;
		std::sort(sorted.begin(), sorted.end());

		std::string key = make_key(sorted);
		if (seen.insert(key).second) 
		{
			uniqueCycles.push_back(cycle);
		}
	}

	indexCycles.swap(uniqueCycles);

	for (auto cycle : indexCycles)
	{
		std::cout << '\n';
		for (auto x : cycle)
		{
			std::cout << x << "  ";
		}
	}


	return {};
}	