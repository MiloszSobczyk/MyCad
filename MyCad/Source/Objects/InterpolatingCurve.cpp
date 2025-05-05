#include "InterpolatingCurve.h"
#include "Core/App.h"

InterpolatingCurve::InterpolatingCurve()
    : renderer(VertexDataType::PositionVertexData),
    bernsteinPolyline(std::make_shared<Polyline>())
{
    name = "InterpolatingCurve_" + std::to_string(id);
    bernsteinPolyline->SetColor(Algebra::Vector4(0.f, 0.8f, 0.8f, 1.f));
}

void InterpolatingCurve::Render()
{
    if (drawBernsteinPolygon)
    {
        bernsteinPolyline->Render();
    }

    if (controlPoints.size() > 1)
    {
        auto shader = ShaderManager::GetInstance().GetShader(ShaderName::BezierCurve);
        shader->Bind();
        shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
        shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
        shader->SetUniformVec4f("u_cameraPos", App::camera.GetPosition());
        shader->SetUniformVec4f("u_zoomLevel", { App::camera.GetZoom(), 0.f, 0.f, 0.f });

        renderer.Render(GL_PATCHES);

        shader->Unbind();
    }
}

void InterpolatingCurve::RenderUI()
{
    Shape::RenderUI();

    ImGui::Checkbox("Draw Berstein polygon", &drawBernsteinPolygon);
    ImGui::Checkbox("Draw interpolating polygon", &drawInterpolatingPolygon);

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

void InterpolatingCurve::AddPoint(const std::shared_ptr<Point>& point)
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

    UpdateCurve();
}

void InterpolatingCurve::RemovePoint(const std::shared_ptr<Point>& point)
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

    UpdateCurve();
}

void InterpolatingCurve::SwapPoints(int index1, int index2)
{
    std::swap(controlPoints[index1], controlPoints[index2]);

    UpdateCurve();
}

void InterpolatingCurve::OnNotified()
{
    UpdateCurve();
}

void InterpolatingCurve::UpdateCurve()
{
    bernsteinPolyline->ClearPoints();

    if (controlPoints.size() < 4)
        return;

    controlPoints.erase(
        std::remove_if(controlPoints.begin(), controlPoints.end(),
            [](const std::weak_ptr<Point>& p) {
                return p.expired();
            }),
        controlPoints.end()
    );
}
