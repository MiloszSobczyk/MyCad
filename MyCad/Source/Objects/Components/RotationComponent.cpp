#include "RotationComponent.h"
#include <imgui/imgui.h>

RotationComponent::RotationComponent()
    : rotation(1.f, 0.f, 0.f, 0.f)
{
}

RotationComponent::RotationComponent(const Algebra::Quaternion& rotation)
    : rotation(rotation.Normalize())
{
}

Algebra::Quaternion RotationComponent::GetRotation() const
{
    return rotation;
}

void RotationComponent::SetRotation(const Algebra::Quaternion& rotation)
{
    this->rotation = rotation.Normalize();
}

void RotationComponent::AddRotation(const Algebra::Quaternion& rotation)
{
    this->rotation = (rotation * this->rotation).Normalize();
}

Algebra::Matrix4 RotationComponent::GetMatrix() const
{
    return this->rotation.ToMatrix();
}

void RotationComponent::RenderUI()
{
    ImGui::Text("Rotation :3");
}
