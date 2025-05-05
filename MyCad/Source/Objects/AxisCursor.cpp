#include "AxisCursor.h"
#include "Core/App.h"
#include "Managers/ShaderManager.h"

#include <imgui/imgui.h>

AxisCursor::AxisCursor() 
    : renderer(VertexDataType::PositionColorVertexData), translationComponent(std::make_shared<TranslationComponent>())
{
    static std::vector<PositionColorVertexData> vertices = {
        {{ -2.f, +0.f, +0.f, 1.f }, { 1.f, 0.f, 0.f, 1.f }},
        {{ +2.f, +0.f, +0.f, 1.f }, { 1.f, 0.f, 0.f, 1.f }},
        {{ +0.f, -2.f, +0.f, 1.f }, { 0.f, 1.f, 0.f, 1.f }},
        {{ +0.f, +2.f, +0.f, 1.f }, { 0.f, 1.f, 0.f, 1.f }},
        {{ +0.f, +0.f, -2.f, 1.f }, { 0.f, 0.f, 1.f, 1.f }},
        {{ +0.f, +0.f, +2.f, 1.f }, { 0.f, 0.f, 1.f, 1.f }},
    };

    static std::vector<unsigned int> indices = {
        0, 1,
        2, 3,
        4, 5,
    };

	renderer.SetVertices(vertices);
	renderer.SetIndices(indices);
    name = "AxisCursor" + std::to_string(id);
}

void AxisCursor::Render()
{
    auto shader = ShaderManager::GetInstance().GetShader(ShaderName::DefaultColor);

    shader->Bind();
    shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
    shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
    shader->SetUniformMat4f("u_modelMatrix", GetModelMatrix());

    renderer.Render(GL_LINES);

    shader->Unbind();
}

void AxisCursor::RenderUI()
{
    ImGui::Text("Axis Cursor Position:");
    auto translation = translationComponent->GetTranslation();
    float axisPos[3] = { translation.x, translation.y, translation.z};

    if (ImGui::InputFloat3("##AxisCursorPos", axisPos))
    {
        translationComponent->SetTranslation(Algebra::Vector4(axisPos[0], axisPos[1], axisPos[2], 1.f));
    }
}

void AxisCursor::HandleInput()
{
    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.x) > FLT_EPSILON || fabs(delta.y) > FLT_EPSILON)
        {
            Algebra::Vector4 direction = Algebra::Vector4(delta.x, 0, -delta.y, 0);

            if (direction.Length() > 0)
            {
                direction = direction.Normalize();
            }

            translationComponent->AddTranslation(direction / 2.f);
        }
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Right))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);

        if (fabs(delta.y) > FLT_EPSILON)
        {
            Algebra::Vector4 direction = Algebra::Vector4(0, -delta.y, 0, 0);

            if (direction.Length() > 0)
            {
                direction = direction.Normalize();
            }
        
            translationComponent->AddTranslation(direction / 2.f);
        }
    }
}

std::shared_ptr<TranslationComponent> AxisCursor::GetTranslationComponent()
{
    return translationComponent;
}

Algebra::Matrix4 AxisCursor::GetModelMatrix() const
{
    return translationComponent->GetMatrix();
}
