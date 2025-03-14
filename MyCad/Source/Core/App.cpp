#include <stdexcept>

#include "App.h"
#include <Engine/VertexBuffer.h>
#include <Engine/VertexArray.h>
#include <Engine/VertexLayout.h>
#include <Engine/Shader.h>


App::App(int windowWidth, int windowHeight, std::string title)
    : window(windowWidth, windowHeight, title), active(true)
{
    active &= InitImgui(window.GetNativeWindow());
    if (!active)
    {
        throw std::runtime_error("cannot initialize app");
    }
}

App::~App()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void App::Run()
{
    std::vector<PositionColorVertex> vertices =
    {
        PositionColorVertex{ .point = Vector4(-0.5f, -0.5f, 0.0f, 1.0f), .color = Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
        PositionColorVertex{ .point = Vector4(0.5f, -0.5f, 0.0f, 1.0f), .color = Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
        PositionColorVertex{ .point = Vector4(0.0f,  0.5f, 0.0f, 1.0f), .color = Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
    };

    VertexBuffer vbo(vertices);

    VertexLayout layout;

    VertexArray vao(vbo, layout);

    Shader shader("Resources/Shaders/Shader.glsl");
    shader.Bind();

    while (active && !window.ShouldClose())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Bind();
        vao.Bind();
        glDrawArrays(GL_TRIANGLES, 0, 3);

        window.Update();
    }
}