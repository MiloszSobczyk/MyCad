#include "SceneSystem.h"

#include "Algebra.h"
#include "Platform/OpenGL/OpenGLApi.h"
#include "Render/Buffer/IndexBuffer.h"
#include "Render/Buffer/VertexBuffer.h"
#include "Render/Shader/ShaderManager.h"
#include <Utils/Logger.h>

#include <iostream>

SceneSystem::SceneSystem()
	: m_Scene{ CreateRef<Scene>() }, m_VertexArray{ CreateRef<VertexArray>() },
    m_CameraSystem(m_Scene)
{
    float vertices[] = {
        // x,    y,    z,    w
        -0.5f, -.5f, 0.0f, 1.0f,
         0.5f, -.5f, 0.0f, 1.0f,
         0.f ,  .5f, 0.0f, 1.0f
    };

    uint32_t indices[] = { 0, 1, 2 };

    // Create buffers and VAO (in real app, do once in init)
    Ref<VertexBuffer> vb = CreateRef<VertexBuffer>(vertices, sizeof(vertices));

    BufferLayout layout = {
        { ShaderDataType::Float4, "position" }
    };
    vb->SetLayout(layout);

    Ref<IndexBuffer> ib = CreateRef<IndexBuffer>(indices, 3);

    m_VertexArray->AddVertexBuffer(vb);
    m_VertexArray->SetIndexBuffer(ib);
}

void SceneSystem::Update()
{
    // Prepare matrices
	Algebra::Vector4 translation(2.0f, 0.0f, -5.0f, 1.0f);
	Algebra::Quaternion rotation(1.0f, 0.0f, 0.0f, 0.0f);

    auto viewMatrix = rotation.ToMatrix() * Algebra::Matrix4::Translation(-translation);
    auto projectionMatrix = Algebra::Matrix4::Projection(
        1650.0f / 960.0f, // Aspect ratio
        100.0f,         // Far plane
        0.1f,         // Near plane
        45.0f         // Field of view in degrees
	);
    auto modelMatrix = Algebra::Matrix4::Identity();

    auto shader = ShaderManager::GetInstance().GetShader(ShaderName::Default);
    shader->Bind();

    shader->SetUniformMat4f("u_projectionMatrix", projectionMatrix);
    shader->SetUniformMat4f("u_viewMatrix", viewMatrix);
    shader->SetUniformMat4f("u_modelMatrix", modelMatrix);

	OpenGLApi::DrawIndexed(m_VertexArray, 3);

    shader->Unbind();
}