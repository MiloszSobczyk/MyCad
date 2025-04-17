#include "SelectedShapes.h"

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
