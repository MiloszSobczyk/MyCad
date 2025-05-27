#pragma once

#include "Shape.h"
#include "Engine/Renderer.h"
#include "Objects/Components/ObservableTranslationComponent.h"

class Point : public std::enable_shared_from_this<Point>, public Shape, public ITranslation, public IObserver, public Observable
{
private:
	std::shared_ptr<ObservableTranslationComponent> translationComponent;

	Renderer<PositionVertexData> renderer;

	std::weak_ptr<Shape> lockShape;
public:
	Point(std::string name = "Point_");
	~Point();
	void Init();

	void Render() override;
	void RenderUI() override;

	std::shared_ptr<TranslationComponent> GetTranslationComponent() override;
	Algebra::Matrix4 GetModelMatrix() const override;

	void OnNotified() override;

	json Serialize() const override;
	static std::shared_ptr<Point> Deserialize(const json& j);

	inline bool IsLocked() { return !lockShape.expired(); }
	inline void Lock(const std::shared_ptr<Shape>& shape) { lockShape = shape; }
};