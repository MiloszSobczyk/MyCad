#include "RenderingSystem.h"

#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"
#include "Managers/ShaderManager.h"
#include "Render/Uniform/UniformManager.h"
#include "Utils/Config.h"


RenderingSystem::RenderingSystem(Ref<Scene> scene)
	: m_Scene{ scene }, m_Renderer{ CreateRef<Renderer>() }
{
}

void RenderingSystem::Update()
{
    auto* viewMatrix = UniformManager::GetInstance().GetUniformValue<Algebra::Matrix4>("u_viewMatrix");
    auto* projectionMatrix = UniformManager::GetInstance().GetUniformValue<Algebra::Matrix4>("u_projectionMatrix");

    m_Renderer->SetPatchParameters(4);

    for (auto e : m_Scene->GetAllEntitiesWith<MeshComponent>(entt::exclude<CurveComponent, PatchComponent, IsInvisibleTag>))
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

        if (e.HasComponent<IsSelectedTag>())
        {
            m_Renderer->SetUniform("u_color", Algebra::Vector4{ .1f, .5f, .2f, 1.f });
        }
        else
        {
            m_Renderer->SetUniform("u_color", Algebra::Vector4{ .5f, .0f, .5f, 1.f });
        }

        const auto& mc = e.GetComponent<MeshComponent>();

		m_Renderer->SetUniform("u_modelMatrix", modelMatrix);
		m_Renderer->SetUniform("u_viewMatrix", *viewMatrix);
		m_Renderer->SetUniform("u_projectionMatrix", *projectionMatrix);
		m_Renderer->SetVertexArray(mc.vertexArray);

        for (const auto& shader : mc.shaders)
        {
		    m_Renderer->SetShader(shader);
		    m_Renderer->Render(mc.renderingMode);
        }
		m_Renderer->ClearUniforms();
    }

    for (auto e : m_Scene->GetAllEntitiesWith<CurveComponent>(entt::exclude<IsInvisibleTag>))
    {
        const auto& mc = e.GetComponent<MeshComponent>();

        m_Renderer->SetUniform("u_cameraPos", Config::InitialCameraPosition);
        m_Renderer->SetUniform("u_viewMatrix", *viewMatrix);
        m_Renderer->SetUniform("u_projectionMatrix", *projectionMatrix);
        m_Renderer->SetVertexArray(mc.vertexArray);

        for (const auto& shader : mc.shaders)
        {
            m_Renderer->SetShader(shader);
            m_Renderer->Render(mc.renderingMode);
        }
        m_Renderer->ClearUniforms();
    }

    m_Renderer->SetPatchParameters(16);

    for (auto e : m_Scene->GetAllEntitiesWith<PatchComponent>(entt::exclude<IsInvisibleTag>))
    {
        const auto& mc = e.GetComponent<MeshComponent>();

        m_Renderer->SetUniform("u_viewMatrix", *viewMatrix);
        m_Renderer->SetUniform("u_projectionMatrix", *projectionMatrix);
        m_Renderer->SetUniform("u_tessLevelU", 8);
        m_Renderer->SetUniform("u_tessLevelV", 8);
        m_Renderer->SetVertexArray(mc.vertexArray);

        for (const auto& shader : mc.shaders)
        {
            m_Renderer->SetShader(shader);
            m_Renderer->Render(mc.renderingMode);
        }
        m_Renderer->ClearUniforms();
    }
}