#pragma once

#include "Algebra.h"
#include "Engine/Renderer.h"
#include "Shape.h"

class AxisCursor : public Shape, public ITranslation
{
private:
	std::shared_ptr<TranslationComponent> translationComponent;

	Renderer<PositionColorVertexData> renderer;
public:
	AxisCursor();
	
	void Render() override;
	void RenderUI() override;
	void HandleInput();

	std::shared_ptr<TranslationComponent> GetTranslationComponent() override;
	Algebra::Matrix4 GetModelMatrix() const override;
};