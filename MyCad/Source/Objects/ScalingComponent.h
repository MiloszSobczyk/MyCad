#pragma once

#include "Algebra.h"

class ScalingComponent
{
private:
    Algebra::Vector4 scaling;

public:
    ScalingComponent();
    ScalingComponent(const Algebra::Vector4& scaling);

    Algebra::Vector4 GetScaling() const;
    void SetScaling(const Algebra::Vector4& scaling);
    void AddScaling(const Algebra::Vector4& scaling);
    Algebra::Matrix4 GetScalingMatrix() const;
};