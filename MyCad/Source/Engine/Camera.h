#pragma once
#include "Algebra.h"

using namespace Algebra;

class Camera
{
private:
	float zoom;
	float yAngle;
	float xAngle;
	float zAngle;

	Vector4 position;
	Matrix4 GetTranslationMatrix();
	Matrix4 GetZoomMatrix();
	Matrix4 GetRotationMatrix();

	void HandleTranslation(const float& delta);
	void HandleZoom(const float& delta);
	void HandleRotations(const float& delta);

public:
	Camera(Vector4 position = Vector4(0.f, 0.f, 0.f, 1.f), float zoom = 1.f);

	Vector4 GetPosition();
	Matrix4 GetViewMatrix();

	void HandleInput();
};