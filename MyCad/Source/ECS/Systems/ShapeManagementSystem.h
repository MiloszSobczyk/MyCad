#pragma once

#include "Core/Scene/Scene.h"
#include "Render/Renderer.h"
#include "Utils/Base.h"

// handles deletion, merging, etc
class ShapeManagementSystem
{
public:
	ShapeManagementSystem(Ref<Scene> scene);

	void Update();

private:
	void MergePoints();

private:
	Ref<Scene> m_Scene;
};