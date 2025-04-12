#include "ScalingComponent.h"

ScalingComponent::ScalingComponent()
    : scaling(1.f, 1.f, 1.f)
{
}

ScalingComponent::ScalingComponent(const Algebra::Vector4& scaling)
    : scaling(scaling)
{
}

Algebra::Vector4 ScalingComponent::GetScaling() const
{
    return scaling;
}

void ScalingComponent::SetScaling(const Algebra::Vector4& scaling)
{
    this->scaling = scaling;
}

void ScalingComponent::AddScaling(const Algebra::Vector4& scaling)
{
    this->scaling = this->scaling.Scale(scaling);
}

Algebra::Matrix4 ScalingComponent::GetScalingMatrix() const
{
    return Algebra::Matrix4::Scale(scaling);
}
