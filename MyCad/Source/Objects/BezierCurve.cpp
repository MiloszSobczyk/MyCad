#include "BezierCurve.h"

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
                // Move to RemovePoint
                polyline->RemovePoint(point);
                changed = true;
                ImGui::PopID();
                break;
            }

            ImGui::SameLine();
            ImGui::Text("%s", point->GetName().c_str());

            if (i > 0)
            {
                ImGui::SameLine();
                if (ImGui::ArrowButton("up", ImGuiDir_Up))
                {
                    std::swap(controlPoints[i], controlPoints[i - 1]);
                    polyline->SwapPoints(i, i - 1);
                    changed = true;
                    ImGui::PopID();
                    break;
                }
            }

            if (i < controlPoints.size() - 1)
            {
                ImGui::SameLine();
                if (ImGui::ArrowButton("down", ImGuiDir_Down))
                {
                    std::swap(controlPoints[i], controlPoints[i + 1]);
                    polyline->SwapPoints(i, i + 1);
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

    UpdateCurve();
}

void BezierCurve::OnNotified()
{
    UpdateCurve();
}

void BezierCurve::UpdateCurve()
{
    polyline->UpdatePoints();
}
