#include "BezierCurve.h"
#include "Core/App.h"

BezierCurve::BezierCurve()
    : renderer(VertexDataType::PositionVertexData),
    polyline(std::make_shared<Polyline>())
{
    name = "BezierCurve" + std::to_string(id);
}

void BezierCurve::Render()
{
    if (drawPolyline)
    {
        polyline->Render();
    }
    if (controlPoints.size() > 1)
    {
        renderer.Render(GL_PATCHES);
    }
}

void BezierCurve::RenderUI()
{
    Shape::RenderUI();

    ImGui::Checkbox("Draw Polyline", &drawPolyline);

    if (controlPoints.empty())
    {
        ImGui::Text("No control points.");
        return;
    }

    bool changed = false;

    ImGui::SeparatorText("Control Points");

    for (size_t i = 0; i < controlPoints.size(); ++i)
    {
        if (auto point = controlPoints[i].lock())
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

            if (i < controlPoints.size() - 1)
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
        UpdateCurve();
    }
}

void BezierCurve::AddPoint(const std::shared_ptr<Point>& point)
{
    if (!point) return;

    for (const auto& p : controlPoints)
    {
        if (auto sp = p.lock())
        {
            if (sp == point)
                return;
        }
    }

    controlPoints.push_back(point);
    point->AddObserver(shared_from_this());

    polyline->AddPoint(point);

    UpdateCurve();
}

void BezierCurve::RemovePoint(const std::shared_ptr<Point>& point)
{
    controlPoints.erase(
        std::remove_if(controlPoints.begin(), controlPoints.end(),
            [&](const std::weak_ptr<Point>& p) {
                auto sp = p.lock();
                return !sp || sp == point;
            }),
        controlPoints.end()
    );

    if (!point)
    {
        point->RemoveObserver(shared_from_this());
    }

    polyline->RemovePoint(point);

    UpdateCurve();
}

void BezierCurve::SwapPoints(int index1, int index2)
{
    std::swap(controlPoints[index1], controlPoints[index2]);
    polyline->SwapPoints(index1, index2);

    UpdateCurve();
}

void BezierCurve::OnNotified()
{
    UpdateCurve();
}

void BezierCurve::UpdateCurve()
{
    if (controlPoints.empty())
    {
        return;
    }
    
    polyline->UpdatePoints();

    std::vector<PositionVertexData> vertices;

    int i = 0;

    for (auto it = controlPoints.begin(); it != controlPoints.end();)
    {
        PositionVertexData vertex;
        if (auto point = it->lock())
        {
            vertex.Position = point->GetTranslationComponent()->GetTranslation();
            vertex.Position.w = 1.f;
            vertices.push_back(vertex);
            ++i;
            if (i % 4 == 0)
            {
                // duplicate for connection with other bezier curve segments
                vertices.push_back(vertex);
                ++i;
            }
            ++it;
        }
        else
        {
            it = controlPoints.erase(it);
        }
    }

    int rest = vertices.size() % 4;

    if (rest == 1)
    {
        auto p = vertices[vertices.size() - 1];
        vertices.push_back(p);
        vertices.push_back(p);
        vertices.push_back(p);
    }
    else if (rest == 2)
    {
        auto p0 = vertices[vertices.size() - 2];
        auto p1 = vertices[vertices.size() - 1];
        vertices.pop_back();
        vertices.pop_back();
        vertices.push_back(p0);
        vertices.push_back(p0);
        vertices.push_back(p1);
        vertices.push_back(p1);
    }
    else if (rest == 3)
    {
        auto p0 = vertices[vertices.size() - 3];
		auto p1 = vertices[vertices.size() - 2];
		auto p2 = vertices[vertices.size() - 1];
		vertices.pop_back();
		vertices.pop_back();
		vertices.pop_back();
		vertices.push_back(p0);
        vertices.push_back(PositionVertexData{ .Position { 1.f / 3.f * p0.Position + 2.f / 3.f * p1.Position } });
        vertices.push_back(PositionVertexData{ .Position { 2.f / 3.f * p1.Position + 1.f / 3.f * p2.Position } });
		vertices.push_back(p2);
    }

    renderer.SetVertices(vertices);
}
