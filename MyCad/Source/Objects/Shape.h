#pragma once

#include <string>

#include "Algebra.h"

class Shape
{
protected:
	Algebra::Vector4 position;
	Algebra::Vector4 color;

	Algebra::Quaternion rotation;
	Algebra::Vector4 translation;
	Algebra::Vector4 scale;

	std::string name;

public:
	virtual ~Shape() = default;

	virtual void HandleInput() = 0;
	virtual void Render() = 0;
	
	inline Algebra::Vector4 GetPosition() { return position; }
	inline Algebra::Vector4 GetColor() { return color; }
	inline void SetPosition(Algebra::Vector4 position) { this->position = position; }
	inline void SetColor(Algebra::Vector4 color) { this->color = color; }

	Algebra::Matrix4 GetRotationMatrix();
	Algebra::Matrix4 GetTranslationMatrix();
	Algebra::Matrix4 GetScaleMatrix();
	Algebra::Matrix4 GetModelMatrix();

	inline void SetRotation(Algebra::Quaternion rotation) { this->rotation = rotation; }
	inline void SetTranslation(Algebra::Vector4 translation) { this->translation = translation; }
	inline void SetScale(Algebra::Vector4 scale) { this->scale = scale; }
	
	inline std::string GetName() { return name; }
	inline void SetName(std::string name) { this->name = name; }
};