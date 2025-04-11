#pragma once

#include "Operations/Operation.h"

class ScalingUniformly : public Operation
{
public:
	ScalingUniformly(std::vector<std::shared_ptr<Shape>>& selected);

	void HandleInput() override;
	void DrawGizmos() override;
};