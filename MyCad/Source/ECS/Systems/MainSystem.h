#pragma once

#include "Core/Scene/Scene.h"
#include "CameraSystem.h"

// TODO:
// Change name to SceneSystem. Consider if separation for SceneSystem and Scene is needed.

class MainSystem
{
public:
	MainSystem();
	~MainSystem() = default;

	MainSystem(const MainSystem&) = delete;
	MainSystem& operator=(const MainSystem&) = delete;

	void Update();

private:
	Scene scene;
	CameraSystem cameraSystem;
};