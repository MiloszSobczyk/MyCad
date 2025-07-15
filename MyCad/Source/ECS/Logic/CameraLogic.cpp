#include "CameraLogic.h"

#include "Algebra.h"

Algebra::Matrix4 CameraLogic::GetViewMatrix(const Algebra::Vector4& translation, const Algebra::Quaternion& rotation) const
{
    Algebra::Matrix4 mirrorZ = Algebra::Matrix4::Scale(1.0f, 1.0f, -1.0f);

    return mirrorZ 
        * rotation.ToMatrix()
        * Algebra::Matrix4::Translation(-translation);
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