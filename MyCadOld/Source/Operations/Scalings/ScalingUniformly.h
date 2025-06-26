#pragma once

#include "Operations/Operation.h"
#include "Objects/SelectedShapes.h"

class ScalingUniformly : public Operation
{
public:
	ScalingUniformly(std::shared_ptr<SelectedShapes> selected);

	void HandleInput() override;
	void DrawGizmos() override;
	void RenderUI() override;
};