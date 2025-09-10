#pragma once

#include "Core/Scene/Scene.h"
#include "Render/Renderer.h"
#include "Utils/Base.h"
#include "Render/Uniform/UniformCalculation.h"

class RenderingSystem
{
public:
	RenderingSystem(Ref<Scene> scene);

	void SetUniformsForEntity(Ref<Renderer> renderer, Ref<Shader> shader, 
		Ref<UniformCalculation> uniformCalculation, Entity entity);

	void Update();

private:
	Ref<Scene> m_Scene;
	Ref<Renderer> m_Renderer;
};