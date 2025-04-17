#pragma once

#include "Algebra.h"
#include "Engine/Renderer.h"
#include "Shape.h"

class AxisCursor : public Shape, public ITranslation
{
private:
	TranslationComponent translationComponent;

	Renderer<PositionColorVertexData> renderer;
public:
	AxisCursor();
	
	void Render() override;
	void RenderUI() override;
	void HandleInput();

	const TranslationComponent& GetTranslationComponent() const override;
	Algebra::Matrix4 GetModelMatrix() const override;
};