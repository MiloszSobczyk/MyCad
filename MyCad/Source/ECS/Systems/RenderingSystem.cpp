#include "RenderingSystem.h"

#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/Uniform/UniformManager.h"


RenderingSystem::RenderingSystem(Ref<Scene> scene)
	: m_Scene{ scene }, m_Renderer{ CreateRef<Renderer>() }
{
}

void RenderingSystem::Update()
{
    for (auto entity : m_Scene->GetAllEntitiesWith<MeshComponent>())
    {
        auto modelMatrix = Algebra::Matrix4::Identity();
        auto* viewMatrix = UniformManager::GetInstance().GetUniformValue<Algebra::Matrix4>("u_viewMatrix");
        auto* projectionMatrix = UniformManager::GetInstance().GetUniformValue<Algebra::Matrix4>("u_projectionMatrix");

        auto shader = ShaderManager::GetInstance().GetShader(ShaderName::Default);

        Entity e{ entity, m_Scene.get() };

        const auto& mc = e.GetComponent<MeshComponent>();
    
		m_Renderer->SetShader(shader);
		m_Renderer->SetModelMatrix(modelMatrix);
		m_Renderer->SetViewMatrix(*viewMatrix);
		m_Renderer->SetProjectionMatrix(*projectionMatrix);
		m_Renderer->SetVertexArray(mc.vertexArray);

		m_Renderer->Render(RenderingMode::Lines);
    }
}