#pragma once

#include "Shape.h"
#include "Engine/Renderer.h"

class Point : public Shape, public ITranslation
{
private:
	TranslationComponent translationComponent;

	// TODO: extract into Shape
	Renderer<PositionVertexData> renderer;

public:
    Point();

	void Render() override;
	void RenderUI() override;

	const TranslationComponent& GetTranslationComponent() const override;
	Algebra::Matrix4 GetModelMatrix() const override;
};