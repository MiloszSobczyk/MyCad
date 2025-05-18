#include "SelectedShapes.h"
#include "BezierSurfaceC0.h"
#include "BezierSurfaceC2.h"

void SelectedShapes::Clear()
{
	for (auto& shape : selectedShapes)
	{
		if (auto surfaceC0 = std::dynamic_pointer_cast<BezierSurfaceC0>(shape))
		{
			surfaceC0->ClearSelection();
		}
		else if (auto surfaceC2 = std::dynamic_pointer_cast<BezierSurfaceC2>(shape))
		{
			surfaceC2->ClearSelection();
		}
	}
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

	if (auto surfaceC0 = std::dynamic_pointer_cast<BezierSurfaceC0>(shape))
	{
		surfaceC0->ClearSelection();
	}
	else if (auto surfaceC2 = std::dynamic_pointer_cast<BezierSurfaceC2>(shape))
	{
		surfaceC2->ClearSelection();
	}
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
