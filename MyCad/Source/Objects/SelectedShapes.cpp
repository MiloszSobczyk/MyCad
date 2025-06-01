#include "SelectedShapes.h"

#include "Objects/Shape.h"
#include "Objects/Point.h"

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

	return newPoint;
}
