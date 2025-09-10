#include "RenderingSystem.h"

#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"
#include "Managers/ShaderManager.h"
#include "Managers/UniformManager.h"
#include "Render/Uniform/UniformManagerOld.h"
#include "Utils/Config.h"

#include <chrono>
#include <iostream>

RenderingSystem::RenderingSystem(Ref<Scene> scene)
	: m_Scene{ scene }, m_Renderer{ CreateRef<Renderer>() }
{
}

void RenderingSystem::SetUniformsForEntity(Ref<Renderer> renderer, Ref<Shader> shader, Ref<UniformCalculation> uniformCalculation, Entity entity)
{
    auto& uniformDefs = shader->GetUniformDefinitions();
    for (auto& [name, def] : uniformDefs)
    {
        UniformValue value = uniformCalculation->Get(name, entity);

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
    auto startTime = std::chrono::high_resolution_clock::now();

    auto* viewMatrix = UniformManagerOld::GetInstance().GetUniformValue<Algebra::Matrix4>("u_viewMatrix");
    auto* projectionMatrix = UniformManagerOld::GetInstance().GetUniformValue<Algebra::Matrix4>("u_projectionMatrix");

    m_Renderer->SetPatchParameters(4);

    for (auto e : m_Scene->GetAllEntitiesWithAny<MeshComponent>(entt::exclude_t<IsInvisibleTag>()))
    {
        m_Renderer->SetUniform("u_viewMatrix", *viewMatrix);
        m_Renderer->SetUniform("u_projectionMatrix", *projectionMatrix);

        const auto& mc = e.GetComponent<MeshComponent>();

        for (const auto& shader : mc.shaders)
        {
            m_Renderer->SetShader(shader);

            SetUniformsForEntity(m_Renderer, shader, mc.uniformCalculation, e);

            m_Renderer->SetVertexArray(mc.vertexArray);

            m_Renderer->Render(mc.renderingMode);
        }

        m_Renderer->ClearUniforms();
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = endTime - startTime;

    std::cout << "[RenderingSystem] Update loop took: " << elapsed.count() << " ms\n";
}
