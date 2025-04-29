#pragma once

#include "Shape.h"
#include "Engine/Renderer.h"
#include "Objects/Components/ObservableTranslationComponent.h"

class Point : public std::enable_shared_from_this<Point>, public Shape, public ITranslation, public IObserver, public Observable
{
private:
	std::shared_ptr<ObservableTranslationComponent> translationComponent;

	// TODO: extract into Shape
	Renderer<PositionVertexData> renderer;
public:
    Point();
	~Point();
	void Init();

	void Render() override;
	void RenderColor();
	void RenderUI() override;

	std::shared_ptr<TranslationComponent> GetTranslationComponent() override;
	Algebra::Matrix4 GetModelMatrix() const override;

	void OnNotified() override;
};