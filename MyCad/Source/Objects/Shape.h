#pragma once

#include <string>
#include <imgui/imgui.h>

#include "Algebra.h"
#include "ScalingComponent.h"
#include "RotationComponent.h"
#include "TranslationComponent.h"

enum class ComponentFlags : uint8_t
{
	None = 0,
	Scaling = 1 << 0,
	Rotation = 1 << 1,
	Translation = 1 << 2,
	All = Scaling | Rotation | Translation
};

inline ComponentFlags operator|(ComponentFlags a, ComponentFlags b)
{
	return static_cast<ComponentFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b));
}

inline bool operator&(ComponentFlags a, ComponentFlags b)
{
	return static_cast<bool>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b));
}


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
	Shape(ComponentFlags flags = ComponentFlags::All);
	virtual ~Shape() = default;

	virtual void Render() = 0;
	
	inline int GetId() { return id; }

	inline Algebra::Vector4 GetColor() { return color; }
	inline void SetColor(Algebra::Vector4 color) { this->color = color; }
	
	inline std::string GetName() { return name; }
	inline void SetName(std::string name) { this->name = name; }

	virtual void SetScaling(const Algebra::Vector4& scaling) const;
	virtual void AddScaling(const Algebra::Vector4& scaling) const;
	Algebra::Vector4 GetScaling() const;
	Algebra::Matrix4 GetScalingMatrix() const;

	virtual void SetRotation(const Algebra::Quaternion& rotation) const;
	virtual void AddRotation(const Algebra::Quaternion& rotation) const;
	Algebra::Quaternion GetRotation() const;
	Algebra::Matrix4 GetRotationMatrix() const;

	virtual void SetTranslation(const Algebra::Vector4& translation) const;
	virtual void AddTranslation(const Algebra::Vector4& translation) const;
	Algebra::Vector4 GetTranslation() const;
	Algebra::Matrix4 GetTranslationMatrix() const;

	virtual void RotateAroundPoint(Algebra::Vector4 point, Algebra::Quaternion q);
	void ScaleAroundPoint(Algebra::Vector4 point, Algebra::Vector4 scaleFactor);

	Algebra::Matrix4 GetModelMatrix() const;

	virtual void RenderUI();
};