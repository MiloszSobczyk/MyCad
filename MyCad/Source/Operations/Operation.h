#pragma once

#include "Objects/Shape.h"

#include <vector>

enum class OperationMode
{
	Local = 0,
	AroundPoint = 1,
	AroundCenter = 2,
	SIZE,
};

struct InputState {
	bool leftMousePressed;
};

class Operation
{
protected:
	std::vector<std::shared_ptr<Shape>>& selected;
public:
	inline Operation(std::vector<std::shared_ptr<Shape>>& selected) 
		: selected(selected) {}
	virtual ~Operation() = default;

	inline void SetSelected(std::vector<std::shared_ptr<Shape>>& selected) { this->selected = selected; }
	virtual void HandleInput() = 0;
	virtual void DrawGizmos() = 0;
};