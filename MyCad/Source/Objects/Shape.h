#pragma once

#include <string>
#include <imgui/imgui.h>

#include "Algebra.h"
#include "ScalingComponent.h"
#include "RotationComponent.h"
#include "TranslationComponent.h"

class Shape
{
private:
	static int idCounter;
protected:
	int id;

	Algebra::Vector4 color;
	std::string name;

	std::unique_ptr<ScalingComponent> scalingComponent;
	std::unique_ptr<RotationComponent> rotationComponent;
	std::unique_ptr<TranslationComponent> translationComponent;

public:
	Shape();
	virtual ~Shape() = default;

	virtual void Render() = 0;
	
	inline int GetId() { return id; }

	inline Algebra::Vector4 GetColor() { return color; }
	inline void SetColor(Algebra::Vector4 color) { this->color = color; }
	
	inline std::string GetName() { return name; }
	inline void SetName(std::string name) { this->name = name; }

	void SetScaling(const Algebra::Vector4& scaling) const;
	void AddScaling(const Algebra::Vector4& scaling) const;
	Algebra::Vector4 GetScaling() const;
	Algebra::Matrix4 GetScalingMatrix() const;

	void SetRotation(const Algebra::Quaternion& rotation) const;
	void AddRotation(const Algebra::Quaternion& rotation) const;
	Algebra::Quaternion GetRotation() const;
	Algebra::Matrix4 GetRotationMatrix() const;

	void SetTranslation(const Algebra::Vector4& translation) const;
	void AddTranslation(const Algebra::Vector4& translation) const;
	Algebra::Vector4 GetTranslation() const;
	Algebra::Matrix4 GetTranslationMatrix() const;

	virtual void RotateAroundPoint(Algebra::Vector4 point, Algebra::Quaternion q);
	void ScaleAroundPoint(Algebra::Vector4 point, Algebra::Vector4 scaleFactor);

	Algebra::Matrix4 GetModelMatrix() const;

	virtual void RenderUI();
};