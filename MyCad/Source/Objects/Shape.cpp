#include "Shape.h"

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
    return Algebra::Matrix4::Scale(scale);
}

Algebra::Matrix4 Shape::GetModelMatrix()
{
    return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
}
