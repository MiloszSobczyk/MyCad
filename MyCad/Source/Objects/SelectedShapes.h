#pragma once

#include <vector>
#include <memory>
#include <optional>

#include "Algebra.h"

class Shape;
class Point;

class SelectedShapes
{
private:
	std::vector<std::shared_ptr<Shape>> selectedShapes;
public:
	void Clear();
	void AddShape(std::shared_ptr<Shape> shape);
	void RemoveShape(std::shared_ptr<Shape> shape);
	void ToggleShape(std::shared_ptr<Shape> shape);
	bool IsSelected(std::shared_ptr<Shape> shape) const;

	inline bool IsEmpty() const { return selectedShapes.empty(); }
	inline int Size() const { return selectedShapes.size(); }

	inline auto begin() { return selectedShapes.begin(); }
	inline auto end() { return selectedShapes.end(); }

	inline auto begin() const { return selectedShapes.begin(); }
	inline auto end() const { return selectedShapes.end(); }

	std::shared_ptr<Shape> GetAt(int index) const;

	std::optional<Algebra::Vector4> GetAveragePosition() const;

	std::optional<std::shared_ptr<Point>> MergePoints();

	template<typename T>
	std::vector<std::shared_ptr<T>> GetSelectedWithType() const;
};

template<typename T>
std::vector<std::shared_ptr<T>> SelectedShapes::GetSelectedWithType() const
{
	std::vector<std::shared_ptr<T>> selected;
	for (const auto& shape : selectedShapes)
	{
		if (std::shared_ptr<T> castShape = std::dynamic_pointer_cast<T>(shape))
		{
			selected.push_back(castShape);
		}
	}
	return selected;
}