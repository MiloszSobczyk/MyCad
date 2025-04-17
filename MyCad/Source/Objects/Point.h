#pragma once

#include "Shape.h"
#include "Engine/Renderer.h"
#include "Objects/Components/ObservableTranslationComponent.h"

class Point : public Shape, public ITranslation
{
private:
	std::shared_ptr<ObservableTranslationComponent> translationComponent;

	// TODO: extract into Shape
	Renderer<PositionVertexData> renderer;

public:
    Point();

	void Render() override;
	void RenderUI() override;

	std::shared_ptr<TranslationComponent> GetTranslationComponent() override;
	Algebra::Matrix4 GetModelMatrix() const override;
};