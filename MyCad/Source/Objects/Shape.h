#pragma once

#include <string>

#include "Algebra.h"

class Shape
{
private:
	static int idCounter;
protected:
	int id;

	Algebra::Vector4 color;

	Algebra::Quaternion rotation;
	Algebra::Vector4 translation;
	Algebra::Vector4 scale;

	std::string name;
public:
	Shape();
	virtual ~Shape() = default;

	virtual void HandleInput() = 0;
	virtual void Render() = 0;
	
	inline int GetId() { return id; }

	inline Algebra::Vector4 GetColor() { return color; }
	inline void SetColor(Algebra::Vector4 color) { this->color = color; }

	Algebra::Matrix4 GetRotationMatrix();
	Algebra::Matrix4 GetTranslationMatrix();
	Algebra::Matrix4 GetScaleMatrix();
	Algebra::Matrix4 GetModelMatrix();

	inline void SetRotation(Algebra::Quaternion rotation) { this->rotation = rotation; }
	inline void AddTranslation(Algebra::Vector4 translation) { this->translation = this->translation + translation; }
	inline void SetScale(Algebra::Vector4 scale) { this->scale = scale; }

	inline std::string GetName() { return name; }
	inline void SetName(std::string name) { this->name = name; }

};