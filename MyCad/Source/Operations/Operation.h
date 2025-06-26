#pragma once

#include "Objects/Shape.h"
#include "Objects/SelectedShapes.h"

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
	std::shared_ptr<SelectedShapes> selected;
public:
	inline Operation(std::shared_ptr<SelectedShapes> selected)
		: selected(selected) {}
	virtual ~Operation() = default;

	inline void SetSelected(std::shared_ptr<SelectedShapes> selected) { this->selected = selected; }
	virtual void HandleInput() = 0;
	virtual void DrawGizmos() = 0;
	virtual void RenderUI() = 0;
};