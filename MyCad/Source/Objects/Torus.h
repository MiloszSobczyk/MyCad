#pragma once

#include "Shape.h"
#include "Engine/Renderer.h"

class Torus : public Shape, public IScaling, public IRotation, public ITranslation
{
private:
	std::shared_ptr<ScalingComponent> scalingComponent;
	std::shared_ptr<RotationComponent> rotationComponent;
	std::shared_ptr<TranslationComponent> translationComponent;

	Renderer<PositionVertexData> renderer;

	float majorRadius = 15.f;
	float minorRadius = 5.f;
	unsigned int majorSegments = 30;
	unsigned int minorSegments = 50;
	bool stateChanged;

	Algebra::Vector4 GetPoint(float angle, float radius);

	void GeneratePoints();

public:
	Torus();

	void Render() override;
	void RenderUI() override;

	std::shared_ptr<ScalingComponent> GetScalingComponent() override;
	std::shared_ptr<RotationComponent> GetRotationComponent() override;
	std::shared_ptr<TranslationComponent> GetTranslationComponent() override;

	Algebra::Matrix4 GetModelMatrix() const override;

	json Serialize() const override;
};