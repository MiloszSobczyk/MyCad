#include "MainSystem.h"

#include "Render/Shader/ShaderManager.h"
#include "Algebra.h"
#include "Render/Buffer/VertexBuffer.h"
#include "Render/Buffer/IndexBuffer.h"

#include <iostream>
#include <Utils/Logger.h>

MainSystem::MainSystem()
	: scene(), cameraSystem(&scene), vertexArray()
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

    vertexArray.AddVertexBuffer(vb);
    vertexArray.SetIndexBuffer(ib);
}

void MainSystem::Update()
{
    // Prepare matrices
    auto viewMatrix = Algebra::Matrix4::Identity();
    viewMatrix[2][2] = 1;
    auto projectionMatrix = Algebra::Matrix4::Identity();
    auto modelMatrix = Algebra::Matrix4::Identity();

    auto shader = ShaderManager::GetInstance().GetShader(ShaderName::Default);
    shader->Bind();

    shader->SetUniformMat4f("u_projectionMatrix", projectionMatrix);
    shader->SetUniformMat4f("u_viewMatrix", viewMatrix);
    shader->SetUniformMat4f("u_modelMatrix", modelMatrix);

    vertexArray.Bind();

    GLCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr));

    vertexArray.Unbind();
    shader->Unbind();
}