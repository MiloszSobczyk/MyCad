#include "AxisCursor.h"
#include <imgui/imgui.h>



AxisCursor::AxisCursor() 
    : renderer(VertexDataType::PositionVertexData)
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

    name = "AxisCursor" + std::to_string(id);
	renderer.SetVertices(vertices);
	renderer.SetIndices(indices);
}

void AxisCursor::Render()
{
	renderer.Render(GL_LINES);
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

            translationComponent.AddTranslation(direction / 2.f);
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
        
            translationComponent.AddTranslation(direction / 2.f);
        }
    }
}

const TranslationComponent& AxisCursor::GetTranslationComponent() const
{
    return translationComponent;
}

Algebra::Matrix4 AxisCursor::GetModelMatrix() const
{
    return translationComponent.GetMatrix();
}
