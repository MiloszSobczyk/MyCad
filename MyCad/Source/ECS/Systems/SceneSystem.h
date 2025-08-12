#pragma once

#include "CameraSystem.h"
#include "MeshGenerationSystem.h"
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
	
	CameraSystem m_CameraSystem;
	MeshGenerationSystem m_MeshGenerationSystem;
};