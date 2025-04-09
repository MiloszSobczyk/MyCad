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
public:
	virtual void Start(std::vector<Shape*>& selected) = 0;
	virtual void Update(InputState input) = 0;
	virtual void Commit() = 0;
	virtual void Cancel() = 0;
	virtual void DrawGizmos() = 0;
};