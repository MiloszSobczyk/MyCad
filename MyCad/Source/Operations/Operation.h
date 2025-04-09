#pragma once

#include "Objects/Shape.h"

#include <vector>

enum class OperationMode
{
	Local = 0,
	Global = 1,
	AroundPoint = 2,
	AroundCenter = 3,
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

	virtual void HandleInput() = 0;
	virtual void DrawGizmos() = 0;
};