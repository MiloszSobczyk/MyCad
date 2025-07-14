#pragma once

#include "Algebra.h"

#include "ECS/Components/CameraComponent.h"

class CameraController
{
public:
    void Update(CameraComponent& camera, const float& deltaTime);

    Algebra::Matrix4 GetViewMatrix(CameraComponent& camera);
    Algebra::Matrix4 GetProjectionMatrix(CameraComponent& camera);

private:
    float pitch = 0.0f;
    float yaw = 0.0f;

    void HandleTranslation(CameraComponent& camera, const float& deltaTime);
    void HandleRotation(CameraComponent& camera, const float& deltaTime);
    void HandleZoom(CameraComponent& camera, const float& deltaTime);
};
