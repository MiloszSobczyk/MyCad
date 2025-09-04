#pragma once

#include "Core/Scene/Scene.h"
#include "Utils/Base.h"

#include "Algebra.h"
#include "Managers/ShaderManager.h"

class MeshGenerationSystem
{
public:
	MeshGenerationSystem(Ref<Scene> scene);

	void Update();

private:
	Ref<Scene> m_Scene;

	void UpdateDirtyTags();
	void UpdatePointMeshes();
	void UpdateTorusMeshes();
	void UpdateLineMeshes();
	void UpdatePatchMeshes();
	void UpdateGregoryPatchMeshes();
};