#include "Polyline.h"

#include <GLFW/glfw3.h>

Polyline::Polyline()
{
    name = "Polyline" + std::to_string(id);

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
}

Polyline::~Polyline()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

void Polyline::Render()
{
    if (indexCount == 0) return;

    glBindVertexArray(vao);
    glDrawElements(GL_LINES, static_cast<GLsizei>(indexCount), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
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
    if (points.empty()) return;

    struct Vertex { Algebra::Vector4 Position; };

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (auto it = points.begin(); it != points.end();)
    {
        if (auto point = it->lock())
        {
            auto position = point->GetTranslationComponent()->GetTranslation();
            position.w = 1.0f;
            vertices.push_back({ position });
            ++it;
        }
        else
        {
            it = points.erase(it);
        }
    }

    for (unsigned int i = 0; i + 1 < vertices.size(); ++i)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
    }

    indexCount = indices.size();

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    glBindVertexArray(0);
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

    if (point)
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
