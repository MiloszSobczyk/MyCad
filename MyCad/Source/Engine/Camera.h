#pragma once

#include "Algebra.h"

class Camera
{
private:
	float zoom;
	Algebra::Vector4 position;
	Algebra::Quaternion rotation;

	Algebra::Matrix4 GetTranslationMatrix();
	Algebra::Matrix4 GetZoomMatrix();
	Algebra::Matrix4 GetRotationMatrix();

	void HandleTranslation(const float& deltaTime);
	void HandleZoom(const float& deltaTime);
	void HandleRotations(const float& deltaTime);

public:
	Camera(Algebra::Vector4 position = Algebra::Vector4(0.f, 0.f, 0.f, 1.f), float zoom = 1.f);

	Algebra::Vector4 GetPosition();
	Algebra::Matrix4 GetViewMatrix();

	void HandleInput();

	Algebra::Vector4 GetUpVector() const;
	Algebra::Vector4 GetRightVector() const;
};