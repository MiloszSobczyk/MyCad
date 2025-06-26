#pragma once

#include "Algebra.h"

class RotationComponent
{
private:
    Algebra::Quaternion rotation;

public:
    RotationComponent();
    RotationComponent(const Algebra::Quaternion& rotation);

    Algebra::Quaternion GetRotation() const;
    void SetRotation(const Algebra::Quaternion& rotation);
    void AddRotation(const Algebra::Quaternion& rotation);
    Algebra::Matrix4 GetMatrix() const;

    void RenderUI();
};