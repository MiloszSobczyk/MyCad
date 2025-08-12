#include "SceneSystem.h"

#include "Algebra.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/MeshComponent.h"
#include "ECS/Components/TorusComponent.h"
#include "ECS/Tags/IsDirtyTag.h"
#include "Platform/OpenGL/OpenGLApi.h"
#include "Render/Buffer/IndexBuffer.h"
#include "Render/Buffer/VertexBuffer.h"
#include "Render/Shader/ShaderManager.h"
#include "Render/Uniform/UniformManager.h"
#include "Utils/Logger.h"

#include <iostream>

SceneSystem::SceneSystem()
	: m_Scene{ CreateRef<Scene>() }, m_CameraSystem(m_Scene), m_MeshGenerationSystem(m_Scene)
{
    Entity entity = m_Scene->CreateEntity();
    entity.EmplaceComponent<TorusComponent>();
    entity.EmplaceTag<IsDirtyTag>();

    m_MeshGenerationSystem.Update();
}

void SceneSystem::Update()
{
    for (auto entity : m_Scene->GetAllEntitiesWith<MeshComponent>())
    {
        auto modelMatrix = Algebra::Matrix4::Identity();
	    auto* viewMatrix = UniformManager::GetInstance().GetUniformValue<Algebra::Matrix4>("u_viewMatrix");
	    auto* projectionMatrix = UniformManager::GetInstance().GetUniformValue<Algebra::Matrix4>("u_projectionMatrix");

        auto shader = ShaderManager::GetInstance().GetShader(ShaderName::Default);
        shader->Bind();

        Entity e{ entity, m_Scene.get() };

        const auto& mc = e.GetComponent<MeshComponent>();

        shader->SetUniformMat4f("u_modelMatrix", modelMatrix);
        shader->SetUniformMat4f("u_viewMatrix", *viewMatrix);
        shader->SetUniformMat4f("u_projectionMatrix", *projectionMatrix);

        OpenGLApi::DrawIndexed(mc.vertexArray, 0);

        shader->Unbind();
    }
}