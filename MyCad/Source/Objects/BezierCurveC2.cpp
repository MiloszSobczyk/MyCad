#include "BezierCurveC2.h"

BezierCurveC2::BezierCurveC2()
    : renderer(VertexDataType::PositionVertexData),
    polyline(std::make_shared<Polyline>())
{
    name = "BezierCurveC2_" + std::to_string(id);
}

void BezierCurveC2::Render()
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

void BezierCurveC2::RenderUI()
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

void BezierCurveC2::AddPoint(const std::shared_ptr<Point>& point)
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

    //polyline->AddPoint(point);

    UpdateCurve();
}

void BezierCurveC2::RemovePoint(const std::shared_ptr<Point>& point)
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

void BezierCurveC2::SwapPoints(int index1, int index2)
{
    std::swap(controlPoints[index1], controlPoints[index2]);
    polyline->SwapPoints(index1, index2);

    UpdateCurve();
}

void BezierCurveC2::OnNotified()
{
    UpdateCurve();
}

void BezierCurveC2::UpdateCurve()
{
    if (controlPoints.size() < 4)
        return;

    polyline->UpdatePoints();

    std::vector<PositionVertexData> vertices;

    controlPoints.erase(
        std::remove_if(controlPoints.begin(), controlPoints.end(),
            [](const std::weak_ptr<Point>& p) {
                return p.expired();
            }),
        controlPoints.end()
    );

    for (size_t i = 0; i + 3 < controlPoints.size(); ++i)
    {
        auto d0 = controlPoints[i].lock();
        auto d1 = controlPoints[i + 1].lock();
        auto d2 = controlPoints[i + 2].lock();
        auto d3 = controlPoints[i + 3].lock();

        Algebra::Vector4 D0 = d0->GetTranslationComponent()->GetTranslation();
        Algebra::Vector4 D1 = d1->GetTranslationComponent()->GetTranslation();
        Algebra::Vector4 D2 = d2->GetTranslationComponent()->GetTranslation();
        Algebra::Vector4 D3 = d3->GetTranslationComponent()->GetTranslation();

        D0.w = 1.0f;
        D1.w = 1.0f;
        D2.w = 1.0f;
        D3.w = 1.0f;

        Algebra::Vector4 P0 = (D0 + 4.0f * D1 + D2) / 6.0f;
        Algebra::Vector4 P1 = (2.0f * D1 + D2) / 3.0f;
        Algebra::Vector4 P2 = (D1 + 2.0f * D2) / 3.0f;
        Algebra::Vector4 P3 = (D1 + 4.0f * D2 + D3) / 6.0f;

        vertices.push_back(PositionVertexData{ .Position = P0 });
        vertices.push_back(PositionVertexData{ .Position = P1 });
        vertices.push_back(PositionVertexData{ .Position = P2 });
        vertices.push_back(PositionVertexData{ .Position = P3 });
    }

    renderer.SetVertices(vertices);
}
