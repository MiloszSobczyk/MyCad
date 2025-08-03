#pragma once

#include "Algebra.h"

#include "ECS/Logic/CameraLogic.h"
#include "Utils/Base.h"

struct CameraComponent 
{
	Scope<CameraLogic> cameraLogic = CreateScope<CameraLogic>();
};
