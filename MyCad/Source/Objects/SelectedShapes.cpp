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
	if (IsEmpty())
	{
		return std::nullopt;
	}

	Algebra::Vector4 result;
	int count = 0;
	for (auto& selected : selectedShapes)
	{
		// TODO:
		//if (selected->HasTranslation())
		//{
		//	result += selected->GetTranslation();
		//	count = 1;
		//}
	}
	
	if (count == 0) 
		return std::nullopt;
	
	result = result / selectedShapes.size();
	return result;
}
