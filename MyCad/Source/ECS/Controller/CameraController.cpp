#include "CameraController.h"

void CameraController::Update(CameraComponent& camera, const float& deltaTime)
{
}

Algebra::Matrix4 CameraController::GetViewMatrix(CameraComponent& camera)
{
	auto view = Algebra::Matrix4::Identity();
	view[1][3] = -20.f;
	view[2][2] = -1.f;
	view[2][3] = -50.f;
	return Algebra::Matrix4();
}

Algebra::Matrix4 CameraController::GetProjectionMatrix(CameraComponent& camera)
{
	return Algebra::Matrix4(
		Algebra::Vector4(0.5625f, 0.f, 0.f, 0.f),
		Algebra::Vector4(0.f, 1.f, 0.f, 0.f),
		Algebra::Vector4(0.f, 0.f, -1.002f, 2.002f),
		Algebra::Vector4(0.f, 0.f, 1.f, 0.f)
	);
}

void CameraController::HandleTranslation(CameraComponent& camera, const float& deltaTime)
{
}

void CameraController::HandleRotation(CameraComponent& camera, const float& deltaTime)
{
}

void CameraController::HandleZoom(CameraComponent& camera, const float& deltaTime)
{
}
