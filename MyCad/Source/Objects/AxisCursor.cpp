#include "AxisCursor.h"
#include <imgui/imgui.h>

std::vector<PositionColorVertexData> AxisCursor::vertices = {
	{{ -2.f, +0.f, +0.f, 1.f }, { 1.f, 0.f, 0.f, 1.f }},
	{{ +2.f, +0.f, +0.f, 1.f }, { 1.f, 0.f, 0.f, 1.f }},
	{{ +0.f, -2.f, +0.f, 1.f }, { 0.f, 1.f, 0.f, 1.f }},
	{{ +0.f, +2.f, +0.f, 1.f }, { 0.f, 1.f, 0.f, 1.f }},
	{{ +0.f, +0.f, -2.f, 1.f }, { 0.f, 0.f, 1.f, 1.f }},
	{{ +0.f, +0.f, +2.f, 1.f }, { 0.f, 0.f, 1.f, 1.f }},
};

std::vector<unsigned int> AxisCursor::indices = {
	0, 1,
	2, 3,
	4, 5,
};

AxisCursor::AxisCursor()
	: Shape(ComponentFlags::Translation), visible(true), renderer(VertexDataType::PositionColorVertexData)
{
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

            SetTranslation(GetTranslation() + direction / 10.f);
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
        
            SetTranslation(GetTranslation() + direction / 10.f);
        }
    }
}
