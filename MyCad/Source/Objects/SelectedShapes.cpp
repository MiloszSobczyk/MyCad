#include "SelectedShapes.h"

void SelectedShapes::Clear()
{
	selectedShapes.clear();
}

void SelectedShapes::AddShape(std::shared_ptr<Shape> shapePtr)
{
	if (IsSelected(shapePtr))
	{
		return;
	}

	selectedShapes.push_back(shapePtr);
}

void SelectedShapes::RemoveShape(std::shared_ptr<Shape> shapePtr)
{
	selectedShapes.erase(std::find(selectedShapes.begin(), selectedShapes.end(), shapePtr));
}

void SelectedShapes::ToggleShape(std::shared_ptr<Shape> shapePtr)
{
	if (IsSelected(shapePtr))
	{
		RemoveShape(shapePtr);
	}
	else
	{
		AddShape(shapePtr);
	}
}

bool SelectedShapes::IsSelected(std::shared_ptr<Shape> shapePtr) const
{
	return std::find(selectedShapes.begin(), selectedShapes.end(), shapePtr) != selectedShapes.end();
}

std::optional<Algebra::Vector4> SelectedShapes::GetAveragePosition() const
{
	if (IsEmpty())
	{
		return std::nullopt;
	}

	Algebra::Vector4 result;
	for (auto& selected : selectedShapes)
	{
		result += selected->GetTranslation();
	}
	result = result / selectedShapes.size();
	return result;
}
