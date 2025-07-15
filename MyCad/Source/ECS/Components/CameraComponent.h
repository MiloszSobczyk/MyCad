#pragma once

#include "Algebra.h"

#include "ECS/Logic/CameraLogic.h"

struct CameraComponent 
{
	std::unique_ptr<CameraLogic> cameraLogic = std::make_unique<CameraLogic>();
};
