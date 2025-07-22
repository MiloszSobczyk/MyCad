#pragma once

#include "CameraSystem.h"
#include "Core/Scene/Scene.h"
#include "Render/Buffer/VertexArray.h"

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
	Ref<Scene> m_Scene;
	Ref<VertexArray> m_VertexArray;
	
	CameraSystem m_CameraSystem;
};