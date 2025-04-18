#include "Polyline.h"

Polyline::Polyline()
	: renderer(VertexDataType::PositionVertexData)
{
    name = "Polyline" + std::to_string(id);
}


void Polyline::Render()
{
    if (points.size() > 1)
    {
        renderer.Render(GL_LINES);
    }
}

void Polyline::RenderUI()
{
    Shape::RenderUI();

    if (points.empty())
    {
        ImGui::Text("No points.");
        return;
    }

    bool changed = false;

    ImGui::SeparatorText("Control Points");

    for (size_t i = 0; i < points.size(); ++i)
    {
        if (auto point = points[i].lock())
        {
            ImGui::PushID(static_cast<int>(i));

            if (ImGui::Button("X"))
            {
                RemovePoint(point);
                changed = true;
                ImGui::PopID();
                break;
            }

            ImGui::SameLine();
            ImGui::Text("%s", point->GetName().c_str());

            if (i < points.size() - 1)
            {
                ImGui::SameLine();
                if (ImGui::ArrowButton("down", ImGuiDir_Down))
                {
                    SwapPoints(i, i + 1);
                    changed = true;
                    ImGui::PopID();
                    break;
                }
            }

            if (i > 0)
            {
                ImGui::SameLine();
                if (ImGui::ArrowButton("up", ImGuiDir_Up))
                {
                    SwapPoints(i, i - 1);
                    changed = true;
                    ImGui::PopID();
                    break;
                }
            }

            ImGui::PopID();
        }
    }

    if (changed)
    {
        UpdatePoints();
    }
}

void Polyline::UpdatePoints()
{
    if (points.empty())
    {
        return;
    }

    std::vector<PositionVertexData> vertices;
    std::vector<unsigned int> indices;

    for (auto it = points.begin(); it != points.end();)
    {
        if (auto point = it->lock())
        {
            auto position = point->GetTranslationComponent()->GetTranslation();
            position.w = 1.f;
            vertices.push_back(PositionVertexData{ .Position = position });
            ++it;
        }
        else
        {
            it = points.erase(it);
        }
    }

    for (unsigned int i = 0; i < vertices.size() - 1; ++i)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    renderer.SetVertices(vertices);
    renderer.SetIndices(indices);
}

void Polyline::AddPoint(const std::shared_ptr<Point>& point)
{
    if (!point) return;

    for (const auto& p : points)
    {
        if (auto sp = p.lock())
        {
            if (sp == point)
                return;
        }
    }

    points.push_back(point);
    point->AddObserver(shared_from_this());
    UpdatePoints();
}

void Polyline::RemovePoint(const std::shared_ptr<Point>& point)
{
    points.erase(
        std::remove_if(points.begin(), points.end(),
            [&](const std::weak_ptr<Point>& p) {
                auto sp = p.lock();
                return !sp || sp == point;
            }),
        points.end()
    );

    if (!point)
    {
        point->RemoveObserver(shared_from_this());
    }

    UpdatePoints();
}

void Polyline::SwapPoints(int index1, int index2)
{
    std::swap(points[index1], points[index2]);

    UpdatePoints();
}

void Polyline::OnNotified()
{
    UpdatePoints();
}