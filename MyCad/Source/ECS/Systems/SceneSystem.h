#pragma once

#include "CameraSystem.h"
#include "Core/Scene/Scene.h"
#include "Render/Buffer/VertexArray.h"

// TODO:
// Change name to SceneSystem. Consider if separation for SceneSystem and Scene is needed.

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