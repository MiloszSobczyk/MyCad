#pragma once

#include "Utils/Base.h"

class Scene;

class CameraSystem
{
public:
	CameraSystem(Ref<Scene> scene);
	~CameraSystem() = default;

	CameraSystem(const CameraSystem&) = delete;
	CameraSystem& operator=(const CameraSystem&) = delete;

	void Update();
private:
	void UpdateCameraUniforms();

	Ref<Scene> m_Scene = nullptr;
};