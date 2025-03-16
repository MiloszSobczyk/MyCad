#include <stdexcept>

#include "App.h"
#include "Engine/VertexBuffer.h"
#include "Engine/VertexArray.h"
#include "Engine/VertexLayout.h"
#include "Engine/Shader.h"
#include "Engine/Camera.h"
#include <Engine/Renderer.h>
#include <Shapes/Torus.h>


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
    Camera camera;

    std::vector<PositionColorVertexData> vertices =
    {
        PositionColorVertexData{ .Position = Vector4(-0.5f, -0.5f, 0.0f, 1.0f), .Color = Vector4(1.0f, 0.0f, 0.0f, 1.0f) },
        PositionColorVertexData{ .Position = Vector4(0.5f, -0.5f, 0.0f, 1.0f),  .Color = Vector4(0.0f, 1.0f, 0.0f, 1.0f) },
        PositionColorVertexData{ .Position = Vector4(0.0f,  0.5f, 0.0f, 1.0f),  .Color = Vector4(0.0f, 0.0f, 1.0f, 1.0f) },
    };

    Renderer renderer(PositionColor, vertices);

    Shader shader("Resources/Shaders/Shader.glsl");
    shader.Bind();

    Torus torus(1.f, 0.3f, 20, 6);
    torus.CalculatePoints();

    while (active && !window.ShouldClose())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        shader.Bind();

        shader.SetUniformMat4f("view", camera.GetViewMatrix());

        renderer.Render(GL_TRIANGLES);

        window.Update();
    }
}