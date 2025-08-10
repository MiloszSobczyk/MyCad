#pragma once

#include "CameraSystem.h"
#include "Core/Scene/Scene.h"
#include "Render/Buffer/VertexArray.h"

class SceneSystem
{
public:
	SceneSystem();
	~SceneSystem() = default;

	SceneSystem(const SceneSystem&) = delete;
	SceneSystem& operator=(const SceneSystem&) = delete;

	void Update();

private:
	Ref<Scene> m_Scene;
	Ref<VertexArray> m_VertexArray;
	
	CameraSystem m_CameraSystem;
};