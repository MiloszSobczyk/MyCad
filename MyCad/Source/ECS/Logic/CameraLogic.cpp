#include "CameraLogic.h"

#include "Algebra.h"

Algebra::Matrix4 CameraLogic::GetViewMatrix(const Algebra::Vector4& translation,
    const Algebra::Quaternion& rotation) const
{
    return rotation.ToMatrix() * Algebra::Matrix4::Translation(-translation);
}

Algebra::Matrix4 CameraLogic::GetProjectionMatrix(float aspect, float f, float n, float fov) const
{
    return Algebra::Matrix4::Projection(aspect, f, n, fov);
}

Algebra::Vector4 CameraLogic::GetUpVector(const Algebra::Quaternion& rotation) const
{
    Algebra::Vector4 up(0.0f, 1.0f, 0.0f, 0.0f);
    return rotation.Rotate(up);
}

Algebra::Vector4 CameraLogic::GetRightVector(const Algebra::Quaternion& rotation) const
{
    Algebra::Vector4 right(1.0f, 0.0f, 0.0f, 0.0f);
    return rotation.Rotate(right);
}