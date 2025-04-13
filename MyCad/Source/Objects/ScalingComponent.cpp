#include "ScalingComponent.h"
#include <imgui/imgui.h>

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

void ScalingComponent::RenderUI()
{
    static float prevScaling = scaling.x;
    float tempScaling = scaling.x;
    if (ImGui::InputFloat("Scaling", &tempScaling))
    {
        if (tempScaling > 0.1f)
        {
            prevScaling = tempScaling;
        }
        else
        {
            tempScaling = prevScaling;
        }
    }
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        scaling = Algebra::Vector4(tempScaling, tempScaling, tempScaling, 0);
    }
}
