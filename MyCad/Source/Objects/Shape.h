#pragma once

#include <string>
#include <imgui/imgui.h>

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
	Algebra::Vector4 scaling;

	std::string name;
public:
	Shape();
	virtual ~Shape() = default;

	virtual void Render() = 0;
	
	inline int GetId() { return id; }

	inline Algebra::Vector4 GetColor() { return color; }
	inline void SetColor(Algebra::Vector4 color) { this->color = color; }

	Algebra::Matrix4 GetRotationMatrix();
	Algebra::Matrix4 GetTranslationMatrix();
	Algebra::Matrix4 GetScaleMatrix();
	Algebra::Matrix4 GetModelMatrix();

	inline void SetRotation(Algebra::Quaternion rotation) { this->rotation = rotation; }
	inline void AddRotation(Algebra::Quaternion rotation) { this->rotation = rotation * this->rotation; }
	
	inline void SetTranslation(Algebra::Vector4 translation) { this->translation = translation; }
	inline void AddTranslation(Algebra::Vector4 translation) { this->translation = this->translation + translation; }
	
	inline void SetScaling(Algebra::Vector4 scaling) { this->scaling = scaling; }
	inline void AddScaling(Algebra::Vector4 scaling) { this->scaling = this->scaling.Scale(scaling); }

	inline std::string GetName() { return name; }
	inline void SetName(std::string name) { this->name = name; }

	virtual void RenderUI();
};