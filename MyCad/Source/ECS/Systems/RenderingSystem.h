#pragma once

#include "Core/Scene/Scene.h"
#include "Render/Renderer.h"
#include "Utils/Base.h"

class RenderingSystem
{
public:
	RenderingSystem(Ref<Scene> scene);

	void Update();

private:
	Ref<Scene> m_Scene;
	Ref<Renderer> m_Renderer;
};