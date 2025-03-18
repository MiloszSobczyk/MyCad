#include "Camera.h"
#include <imgui/imgui.h>
#include <algorithm>
#include "Core/Globals.h"

Camera::Camera(Algebra::Vector4 position, float zoom)
    : position(position), zoom(zoom), rotation(1, 0, 0, 0)
{
}

Algebra::Matrix4 Camera::GetTranslationMatrix()
{
    return Algebra::Matrix4::Translation(-position.x, -position.y, -position.z);
}

Algebra::Matrix4 Camera::GetZoomMatrix()
{
    return Algebra::Matrix4::Scale(zoom, zoom, zoom);
}

Algebra::Matrix4 Camera::GetRotationMatrix()
{
    return rotation.ToMatrix();
}

void Camera::HandleTranslation(const float& deltaTime)
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Middle))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Middle);
        Algebra::Vector4 direction = Algebra::Vector4(delta.x, -delta.y, 0, 0);

        if (direction.Length() > 0)
        {
            direction = direction.Normalize();
        }

        position += direction * 100.f * deltaTime * GetRotationMatrix() * GetZoomMatrix();
    }
}

void Camera::HandleZoom(const float& deltaTime)
{
    if (ImGui::GetIO().MouseWheel != 0.f)
    {
        zoom += ImGui::GetIO().MouseWheel * 0.1f;
        zoom = std::clamp(zoom, 0.1f, 5.0f);
    }
}

void Camera::HandleRotations(const float& deltaTime)
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
        float yawDelta = delta.x / Globals::StartingWidth * 3.f;
        float pitchDelta = delta.y / Globals::StartingHeight * 3.f;

        Algebra::Quaternion yawQuat = Algebra::Quaternion::CreateFromAxisAngle(Algebra::Vector4(0, 1, 0, 0), yawDelta);

        Algebra::Quaternion tempRotation = (yawQuat * rotation).Normalize();

        Algebra::Vector4 right = tempRotation.Rotate(Algebra::Vector4(1, 0, 0, 0));

        Algebra::Quaternion pitchQuat = Algebra::Quaternion::CreateFromAxisAngle(right, -pitchDelta);

        rotation = (pitchQuat * tempRotation).Normalize();

        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Left);
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
        float rollDelta = delta.x / Globals::StartingWidth * 3.f;

        Algebra::Vector4 forward = rotation.Rotate(Algebra::Vector4(0, 0, -1, 0));

        Algebra::Quaternion rollQuat = Algebra::Quaternion::CreateFromAxisAngle(forward, rollDelta);
        rotation = (rollQuat * rotation).Normalize();

        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
    }
}


Algebra::Vector4 Camera::GetPosition()
{
    return position;
}

Algebra::Matrix4 Camera::GetViewMatrix()
{
    return GetRotationMatrix() * GetTranslationMatrix() * GetZoomMatrix();
}

void Camera::HandleInput()
{
    const float deltaTime = ImGui::GetIO().DeltaTime;
    HandleTranslation(deltaTime);
    HandleZoom(deltaTime);
    HandleRotations(deltaTime);
}
