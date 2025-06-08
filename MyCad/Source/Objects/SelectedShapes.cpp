#include "SelectedShapes.h"

#include "Objects/Shape.h"
#include "Objects/Point.h"
#include "Objects/BezierSurfaceC0.h"

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

static bool SharesEndpoint(const PatchEdge& e1, const PatchEdge& e2)
{
	auto s1 = e1.GetStart().lock().get();
	auto e1p = e1.GetEnd().lock().get();
	auto s2 = e2.GetStart().lock().get();
	auto e2p = e2.GetEnd().lock().get();
	return (s1 && e2p && s1 == e2p) ||
		(e1p && s2 && e1p == s2);
}

std::vector<std::array<PatchEdge, 3>> SelectedShapes::FindEdgeCycles()
{
	auto bezierSurfaces = GetSelectedWithType<BezierSurfaceC0>();

	if (bezierSurfaces.empty())
	{
		return {};
	}

	std::vector<PatchEdge> edgeVertices;
	for (auto& surface : bezierSurfaces)
	{
		auto edges = surface->GetOutsideEdges();
		edgeVertices.insert(edgeVertices.end(), edges.begin(), edges.end());
	}

	for (int i = 0; i < edgeVertices.size(); ++i)
	{
		if (i % 8 == 0)
			std::cout << '\n';

		std::cout << i << "   ";
		edgeVertices[i].Print();
	}
	std::cout << '\n';

	int N = edgeVertices.size();
	std::vector<std::vector<bool>> edgeEdges(N, std::vector<bool>(N, false));

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
			std::cout << edgeEdges[i][j] << "  ";
		}
		std::cout << '\n';
	}

	std::vector<std::array<int, 3>> indexCycles;

	for (int i = 0; i < edgeVertices.size(); ++i)
	{
		for (int j = 0; j < edgeVertices.size(); ++j)
		{
			if (j == i) continue;
			for (int k = 0; k < edgeVertices.size(); ++k)
			{
				if (k == i || k == j) continue;
				if (edgeEdges[i][j] && edgeEdges[j][k] && edgeEdges[k][i])
				{
					indexCycles.push_back({ i, j, k });
				}
			}
		}
	}

	for (auto cycle : indexCycles)
	{
		for (auto x : cycle)
		{
			std::cout << x << "  ";
		}
		std::cout << '\n';
	}


	return {};
}	