#include "RenderingSystem.h"

#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"
#include "Managers/ShaderManager.h"
#include "Managers/UniformManager.h"
#include "Render/Uniform/UniformManagerOld.h"
#include "Utils/Config.h"


RenderingSystem::RenderingSystem(Ref<Scene> scene)
	: m_Scene{ scene }, m_Renderer{ CreateRef<Renderer>() }
{
}

inline void SetUniformsForEntity(Ref<Renderer> renderer, Ref<Shader> shader, Entity entity)
{
    auto& uniformDefs = shader->GetUniformDefinitions();
    for (auto& [name, def] : uniformDefs)
    {
        UniformValue value = UniformManager::GetInstance().GetUniformValue(entity, name);

        switch (def.type)
        {
        case UniformType::Int:
            if (auto v = std::get_if<int>(&value))
                renderer->SetUniform(name, *v);
            break;
        case UniformType::Vec4:
            if (auto v = std::get_if<Algebra::Vector4>(&value))
                renderer->SetUniform(name, *v);
            break;
        case UniformType::Mat4:
            if (auto v = std::get_if<Algebra::Matrix4>(&value))
                renderer->SetUniform(name, *v);
            break;
        }
    }
}



void RenderingSystem::Update()
{
    auto* viewMatrix = UniformManagerOld::GetInstance().GetUniformValue<Algebra::Matrix4>("u_viewMatrix");
    auto* projectionMatrix = UniformManagerOld::GetInstance().GetUniformValue<Algebra::Matrix4>("u_projectionMatrix");

    m_Renderer->SetPatchParameters(4);

    for (auto e : m_Scene->GetAllEntitiesWith<PointComponent>(entt::exclude<CurveComponent, PatchComponent, IsInvisibleTag>))
    {
        m_Renderer->SetUniform("u_viewMatrix", *viewMatrix);
        m_Renderer->SetUniform("u_projectionMatrix", *projectionMatrix);

        const auto& mc = e.GetComponent<MeshComponent>();

        for (const auto& shader : mc.shaders)
        {
            m_Renderer->SetShader(shader);

            SetUniformsForEntity(m_Renderer, shader, e);

            m_Renderer->SetVertexArray(mc.vertexArray);

            m_Renderer->Render(mc.renderingMode);
        }

        m_Renderer->ClearUniforms();
    }



    //for (auto e : m_Scene->GetAllEntitiesWith<CurveComponent>(entt::exclude<IsInvisibleTag>))
    //{
    //    const auto& mc = e.GetComponent<MeshComponent>();

    //    m_Renderer->SetUniform("u_cameraPos", Config::InitialCameraPosition);
    //    m_Renderer->SetUniform("u_viewMatrix", *viewMatrix);
    //    m_Renderer->SetUniform("u_projectionMatrix", *projectionMatrix);
    //    m_Renderer->SetVertexArray(mc.vertexArray);

    //    for (const auto& shader : mc.shaders)
    //    {
    //        m_Renderer->SetShader(shader);
    //        m_Renderer->Render(mc.renderingMode);
    //    }
    //    m_Renderer->ClearUniforms();
    //}

    //m_Renderer->SetPatchParameters(16);

    //for (auto e : m_Scene->GetAllEntitiesWith<PatchComponent>(entt::exclude<IsInvisibleTag>))
    //{
    //    const auto& mc = e.GetComponent<MeshComponent>();

    //    m_Renderer->SetUniform("u_viewMatrix", *viewMatrix);
    //    m_Renderer->SetUniform("u_projectionMatrix", *projectionMatrix);
    //    m_Renderer->SetUniform("u_tessLevelU", 8);
    //    m_Renderer->SetUniform("u_tessLevelV", 8);
    //    m_Renderer->SetVertexArray(mc.vertexArray);

    //    for (const auto& shader : mc.shaders)
    //    {
    //        m_Renderer->SetShader(shader);
    //        m_Renderer->Render(mc.renderingMode);
    //    }
    //    m_Renderer->ClearUniforms();
    //}
}