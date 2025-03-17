#include <stdexcept>

#include "App.h"
#include "Engine/VertexBuffer.h"
#include "Engine/VertexArray.h"
#include "Engine/VertexLayout.h"
#include "Engine/Shader.h"
#include "Engine/Camera.h"
#include <Engine/Renderer.h>
#include <Shapes/Torus.h>

using namespace Algebra;

Vector4 App::GetMousePoint(float x, float y)
{
    float screenWidth = static_cast<float>(window.GetWidth());
    float screenHeight = static_cast<float>(window.GetHeight());
    float scale = fminf(screenHeight, screenWidth) - 1.f;

    x = (2.f * x - screenWidth + 1.f) / scale;
    y = (2.f * y - screenHeight + 1.f) / -scale;

    float z = 0;
    float d = x * x + y * y;
    if (d <= 1.f / 2.f)
    {
        z = sqrtf(1 - d);
    }
    else
    {
        z = 1.f / 2.f / sqrtf(d);
    }

    return Algebra::Vector4(x, y, z, 0);
}

App::App(int windowWidth, int windowHeight, std::string title)
    : window(windowWidth, windowHeight, title), 
    active(true),
    camera(Vector4(0.f, 20.f, -50.f, 1.f), 1.f),
    shader("Resources/Shaders/Shader.glsl")
{
    active &= InitImgui(window.GetNativeWindow());
    if (!active)
    {
        throw std::runtime_error("cannot initialize app");
    }

    viewMatrix = Matrix4::Identity();
    HandleResize();
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
    while (active && !window.ShouldClose())
    {
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        HandleInput();
        Update();

        DisplayParameters();
        Render();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        window.Update();
    }
}

void App::Render()
{
    shader.Bind();
    shader.SetUniformMat4f("u_viewMatrix", camera.GetViewMatrix());
    shader.SetUniformMat4f("u_projectionMatrix", projectionMatrix);
    torus.Render();
    shader.Unbind();
}

void App::HandleInput()
{
    if (ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    {
        return;
    }

    camera.HandleInput();

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        auto mousePos = ImGui::GetMousePos();
        draggingPoint = GetMousePoint(mousePos.x, mousePos.y).Normalize();
        return;
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);

        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        auto mousePos = ImGui::GetMousePos();
        Algebra::Vector4 q = GetMousePoint(mousePos.x, mousePos.y).Normalize();
        if (q == draggingPoint)
        {
            return;
        }
        float theta = acosf(draggingPoint * q);
        auto w = Vector4::Cross(q, draggingPoint).Normalize();
        auto tempMat = Matrix4(Vector4(0.f, 0.f, 0.f, 0.f));
        tempMat[1][0] = w.z;
        tempMat[0][1] = -w.z;
        tempMat[0][2] = -w.y;
        tempMat[2][0] = w.y;
        tempMat[2][1] = w.x;
        tempMat[1][2] = -w.x;
        auto rotation = Matrix4::Identity() + sinf(theta) * tempMat + ((1.f - cosf(theta)) * tempMat * tempMat);
        draggingPoint = q;
    }
}

void App::HandleResize()
{
    float newWidth = static_cast<float>(window.GetWidth() - Globals::RightInterfaceWidth);
    float newHeight = static_cast<float>(window.GetHeight());
    float aspect = newWidth / newHeight;
    projectionMatrix = Algebra::Matrix4::Projection(aspect, 0.1f, 10000.0f, 3.14f / 2.f);
}

void App::Update()
{
}

void App::DisplayParameters()
{
}
