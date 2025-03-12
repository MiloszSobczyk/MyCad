#include "UserInterface.h"
#include <iostream>
#include "Matrix4.h"

using namespace Algebra;

UIValues UserInterface::values{
    .a = 1.f,
    .b = 1.f,
    .c = 1.f,
    .scale = 1.f,
    .translationX = 0.f,
    .translationY = 0.f,
    .translationZ = 0.f,
    .rotation = Matrix4::Identity(),
    .moving = false,
    .raySize = 1,
    .lightIntensity = 5,
};

void UserInterface::HandleMouseDrag()
{
    values.moving = false;

    if (ImGui::IsAnyItemActive() || ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow))
    {
        return;
    }

    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
    {
        auto mousePos = ImGui::GetMousePos();
        startPos = Project(mousePos.x, mousePos.y).Normalize();
        values.moving = true;
        return;
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        auto mousePos = ImGui::GetMousePos();
        std::cout << mousePos.x << ' ' << mousePos.y << '\n';
        Vector4 q = Project(mousePos.x, mousePos.y).Normalize();
        float theta = acosf(startPos * q) * 2.5f;
        auto w = q.Cross(startPos).Normalize();
        auto temp = Matrix4();
        temp[1][0] = w.z;
        temp[0][1] = -w.z;
        temp[0][2] = -w.y;
        temp[2][0] = w.y;
        temp[2][1] = w.x;
        temp[1][2] = -w.x;

        auto quat = Matrix4::Identity() + sinf(theta) * temp + ((1.f - cosf(theta)) * temp * temp);

        values.moving = true;
        values.rotation = quat;
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);

        values.translationX += mouseDelta.x * 0.01f;
        values.translationY -= mouseDelta.y * 0.01f;

        values.translationX = std::max(std::min(values.translationX, 10.f), -10.f);
        values.translationY = std::max(std::min(values.translationY, 10.f), -10.f);

        ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);

        values.moving = true;
    }
}

UserInterface::UserInterface(Window* window)
	: window(window), flags(0), width(200.f)
{
    flags |= ImGuiWindowFlags_NoTitleBar;
    flags |= ImGuiWindowFlags_NoMove;
    flags |= ImGuiWindowFlags_NoResize;
    flags |= ImGuiWindowFlags_NoCollapse;
    flags |= ImGuiWindowFlags_NoDocking;
}

void UserInterface::Render()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    HandleMouseDrag();

    ImGui::SetNextWindowPos(ImVec2(window->GetWidth() - width, 0));
    ImGui::SetNextWindowSize(ImVec2(width, window->GetHeight()));
    ImGui::SetNextWindowBgAlpha(1.0f);

    RenderComponents();

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UserInterface::RenderComponents()
{
    ImGui::Begin("Hello, ImGui!", nullptr, flags);

    ImGui::Text("Ellipse parameters");

    if (ImGui::BeginTable("table1", 2, ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableNextColumn();
        ImGui::Text("a");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##a", &(values.a), 0.3f, 3.0f, "%.2f");

        ImGui::TableNextColumn();
        ImGui::Text("b");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##b", &(values.b), 0.3f, 3.0f, "%.2f");

        ImGui::TableNextColumn();
        ImGui::Text("c");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##c", &(values.c), 0.3f, 3.0f, "%.2f");

        ImGui::EndTable();
    }

    ImGui::Text("Translations");

    if (ImGui::BeginTable("table2", 2, ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableNextColumn();
        ImGui::Text("X");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##x", &(values.translationX), -10.f, 10.f, "%.2f");

        ImGui::TableNextColumn();
        ImGui::Text("Y");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##y", &(values.translationY), -10.f, 10.f, "%.2f");

        ImGui::TableNextColumn();
        ImGui::Text("Z");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##z", &(values.translationZ), -10.f, 10.f, "%.2f");

        ImGui::EndTable();
    }

    ImGui::Text("Scale");

    if (ImGui::BeginTable("table3", 2, ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableNextColumn();
        ImGui::Text("s");
        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::SliderFloat("##scale", &(values.scale), 0.5f, 4.f, "%.2f");

        ImGui::EndTable();
    }

    ImGui::Text("Ray size");

    if (ImGui::BeginTable("table3", 2, ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableNextColumn();
        ImGui::Text("s");
        ImGui::TableNextColumn();

        ImGui::SetNextItemWidth(-FLT_MIN);
        static int scaleIndex = 3;
        static const float scaleValues[] = { 1.f, 2.f, 4.f, 8.f, 16.f, 32.f, 64.f };
        const int maxIndex = (sizeof(scaleValues) / sizeof(scaleValues[0])) - 1;

        if (ImGui::Button("-")) 
        {
            if (scaleIndex > 0) 
                scaleIndex--;
        }
        ImGui::SameLine();
        ImGui::Text("%.0f", scaleValues[scaleIndex]);
        ImGui::SameLine();
        if (ImGui::Button("+")) 
        {
            if (scaleIndex < maxIndex) 
                scaleIndex++;
        }

        values.raySize = scaleValues[scaleIndex];

        ImGui::EndTable();
    }

    ImGui::Text("Light Intensity");

    if (ImGui::BeginTable("table3", 2, ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableNextColumn();
        ImGui::Text("m");
        ImGui::TableNextColumn();

        ImGui::SetNextItemWidth(-FLT_MIN);
        static int lightIntensity = 5;

        if (ImGui::Button("-")) 
        {
            if (lightIntensity > 1) 
                lightIntensity--;
        }

        ImGui::SameLine();
        ImGui::Text("%d", lightIntensity);
        ImGui::SameLine();

        if (ImGui::Button("+")) 
        {
            lightIntensity++;
        }

        values.lightIntensity = lightIntensity;

        ImGui::EndTable();
    }
}

Vector4 UserInterface::Project(float x, float y)
{
    float res = fminf(window->GetWidth(), window->GetHeight()) - 1.f;

    x = 2.f * x / window->GetWidth() - 1.f;
    y = 1.f - 2.f * y / window->GetHeight();

    float d = x * x + y * y;
    float z = 0.f;
    if (d <= 0.5f)
        z = std::sqrtf(1.f - d);
    else
        z = 0.5f / std::sqrtf(d);

    return Vector4(x, y, z, 0.f);
}

