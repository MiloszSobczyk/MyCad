#pragma once

#include "Core/Scene/Scene.h"
#include "Utils/Base.h"

class MeshGenerationSystem
{
public:
	MeshGenerationSystem(Ref<Scene> scene);

	void Update();

private:
	Ref<Scene> m_Scene;

	void UpdateDirtyTags();

	void UpdateTorusMeshes();
	void UpdatePointMeshes();
	void UpdateLineMeshes();
};