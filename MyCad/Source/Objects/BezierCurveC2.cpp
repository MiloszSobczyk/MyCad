#include "BezierCurveC2.h"
#include "Core/App.h"

BezierCurveC2::BezierCurveC2()
    : renderer(VertexDataType::PositionVertexData),
    polyline(std::make_shared<Polyline>()),
	bernsteinPolyline(std::make_shared<Polyline>())
{
    name = "BezierCurveC2_" + std::to_string(id);
}

void BezierCurveC2::Render()
{
    if (drawDeBoorPoints)
    {
        polyline->Render();
    }
    if (drawBernsteinPolygon)
    {
		bernsteinPolyline->Render();
    }
    if (drawBernsteinBase)
    {
		for (const auto point : bernsteinPoints)
		{
			point->Render();
		}
    }
    if (controlPoints.size() > 1)
    {
        auto shaderBezier = ShaderManager::GetInstance().GetShader(ShaderName::BezierCurve);
        shaderBezier->Bind();
        shaderBezier->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
        shaderBezier->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
        shaderBezier->SetUniformVec4f("u_cameraPos", App::camera.GetPosition());
        shaderBezier->SetUniformVec4f("u_zoomLevel", { App::camera.GetZoom(), 0.f, 0.f, 0.f });

        renderer.Render(GL_PATCHES);
    }
}

void BezierCurveC2::RenderUI()
{
    Shape::RenderUI();

    ImGui::Checkbox("Draw de Boor points", &drawDeBoorPoints);
    ImGui::Checkbox("Draw Bernstein base", &drawBernsteinBase);
    ImGui::Checkbox("Draw Berstein polygon", &drawBernsteinPolygon);

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

    ImGui::SeparatorText("Bernstein Points");

    if (bernsteinPoints.empty())
    {
        ImGui::Text("No bernstein points available.");
    }
    else
    {
        int index = 0;
        for (const auto& point : bernsteinPoints)
        {
            bool isSelected = (selectedBernsteinIndex == index);
            std::string label = "B" + std::to_string(index) + "##bernstein" + std::to_string(index);

            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                selectedBernsteinIndex = index;
            }

            ++index;
        }

        if (selectedBernsteinIndex >= 0 && selectedBernsteinIndex < static_cast<int>(bernsteinPoints.size()))
        {
            auto selectedPoint = bernsteinPoints[selectedBernsteinIndex];
            if (selectedPoint)
            {
                auto pos = selectedPoint->GetTranslationComponent()->GetTranslation();
                ImGui::Text("Selected: B%d", selectedBernsteinIndex);
                ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
            }
        }
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

    polyline->AddPoint(point);

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

void BezierCurveC2::HandleInput()
{
    if (selectedBernsteinIndex == -1 || selectedBernsteinIndex > bernsteinPoints.size())
    {
        selectedBernsteinIndex = -1;
        return;
    }


    static const std::vector<std::pair<AxisMode, ImGuiKey>> axisModeMap = {
        { AxisMode::X, ImGuiKey_X },
        { AxisMode::Y, ImGuiKey_Y },
        { AxisMode::Z, ImGuiKey_Z }
    };

    for (const auto& axis : axisModeMap)
    {
        if (ImGui::IsKeyPressed(axis.second))
        {
            axisMode = axis.first;
        }
    }

    if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        ImVec2 delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);

        if (fabs(delta.x) > FLT_EPSILON || fabs(delta.y) > FLT_EPSILON)
        {
            auto normDelta = Algebra::Vector4(delta.x, -delta.y, 0.f, 0.f).Normalize();
            Algebra::Vector4 translation(0, 0, 0, 0);
            translation[static_cast<int>(axisMode)] = axisMode == AxisMode::Y ? normDelta.y : normDelta.x;
	        auto selectedPoint = bernsteinPoints[selectedBernsteinIndex];
            selectedPoint->GetTranslationComponent()->AddTranslation(translation);
        }
    }
}

void BezierCurveC2::UpdateCurve()
{
    bernsteinPoints.clear();
	bernsteinPolyline->ClearPoints();
    polyline->UpdatePoints();
    
    if (controlPoints.size() < 4)
        return;

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

        Algebra::Vector4 bezierPoints[4] = {
            (D0 + 4.0f * D1 + D2) / 6.0f,
            (2.0f * D1 + D2) / 3.0f,
            (D1 + 2.0f * D2) / 3.0f,

            (D1 + 4.0f * D2 + D3) / 6.0f
        };

        for (const auto& p : bezierPoints)
        {
            vertices.push_back(PositionVertexData{ .Position = p });
            
            auto pointPtr = std::make_shared<Point>();
            pointPtr->Init();
            pointPtr->GetTranslationComponent()->SetTranslation(p);
			bernsteinPoints.push_back(pointPtr);
			bernsteinPolyline->AddPoint(pointPtr);
        }
    }

    renderer.SetVertices(vertices);
}
