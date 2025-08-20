#include "RenderingSystem.h"

#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"
#include "Managers/ShaderManager.h"
#include "Render/Uniform/UniformManager.h"


RenderingSystem::RenderingSystem(Ref<Scene> scene)
	: m_Scene{ scene }, m_Renderer{ CreateRef<Renderer>() }
{
}

void RenderingSystem::Update()
{
    for (auto e : m_Scene->GetAllEntitiesWith<MeshComponent>())
    {
		auto modelMatrix = Algebra::Matrix4::Identity();

        if (e.HasComponent<ScalingComponent>())
        {
            const auto& scaling = e.GetComponent<ScalingComponent>();
            modelMatrix = Algebra::Matrix4::Scale(scaling.scaling) * modelMatrix;
        }

        if (e.HasComponent<RotationComponent>())
        {
            const auto& rotation = e.GetComponent<RotationComponent>();
            modelMatrix = Algebra::Matrix4::Rotation(rotation.rotation) * modelMatrix;
		}

        if (e.HasComponent<TranslationComponent>())
        {
            const auto& translation = e.GetComponent<TranslationComponent>();
            modelMatrix = Algebra::Matrix4::Translation(translation.translation) * modelMatrix;
		}

        auto* viewMatrix = UniformManager::GetInstance().GetUniformValue<Algebra::Matrix4>("u_viewMatrix");
        auto* projectionMatrix = UniformManager::GetInstance().GetUniformValue<Algebra::Matrix4>("u_projectionMatrix");

        const auto& mc = e.GetComponent<MeshComponent>();

        auto shader = mc.shader;
    
		m_Renderer->SetShader(shader);
		m_Renderer->SetModelMatrix(modelMatrix);
		m_Renderer->SetViewMatrix(*viewMatrix);
		m_Renderer->SetProjectionMatrix(*projectionMatrix);
		m_Renderer->SetVertexArray(mc.vertexArray);

		m_Renderer->Render(mc.renderingMode);
    }
}