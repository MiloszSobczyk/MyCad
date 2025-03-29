#include "Shape.h"

int Shape::idCounter = 0;

Shape::Shape() 
	: id(++idCounter), color(0.5f, 0.f, 0.5f, 1.0f), scaling(1.f, 1.f, 1.f)
{
}

Algebra::Matrix4 Shape::GetRotationMatrix()
{
    return rotation.ToMatrix();
}

Algebra::Matrix4 Shape::GetTranslationMatrix()
{
    return Algebra::Matrix4::Translation(translation);
}

Algebra::Matrix4 Shape::GetScaleMatrix()
{
    return Algebra::Matrix4::Scale(scaling);
}

Algebra::Matrix4 Shape::GetModelMatrix()
{
	auto t = GetTranslationMatrix();
	auto r = GetRotationMatrix();
	auto s = GetScaleMatrix();

    return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
}
