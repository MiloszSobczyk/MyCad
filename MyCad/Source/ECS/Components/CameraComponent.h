#pragma once

#include "Algebra.h"

#include "ECS/Logic/CameraLogic.h"
#include "Utils/Base.h"

struct CameraComponent 
{
	float fov = 45.0f;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	Scope<CameraLogic> cameraLogic = CreateScope<CameraLogic>();
};
