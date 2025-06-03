#include "BezierCurveC2.h"
#include "Core/App.h"
#include "Managers/IdManager.h"
#include <memory>

BezierCurveC2::BezierCurveC2()
    : renderer(VertexDataType::PositionVertexData),
    polyline(std::make_shared<Polyline>()),
	bernsteinPolyline(std::make_shared<Polyline>()),
    axisMode(AxisMode::X)
{
    name = "BezierCurveC2_" + std::to_string(id);
    bernsteinPolyline->SetColor(ColorPalette::Get(Color::Teal));
}

void BezierCurveC2::Render()
{
    if(somethingChanged)
    {
        somethingChanged = false;
        UpdateCurve();
    }
    if (drawDeBoorPolygon)
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
        auto shader = ShaderManager::GetInstance().GetShader(ShaderName::BezierCurve);
        shader->Bind();
        shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
        shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
        shader->SetUniformVec4f("u_cameraPos", App::camera.GetPosition());

        renderer.Render(GL_PATCHES);

        shader->Unbind();
    }
}

void BezierCurveC2::RenderUI()
{
    Shape::RenderUI();

    ImGui::Checkbox("Draw de Boor polygon", &drawDeBoorPolygon);
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
        const char* axisNames[] = { "X", "Y", "Z" };
        ImGui::SeparatorText("Transformation Axis");
        ImGui::Text("Current axis: %s", axisNames[static_cast<int>(axisMode)]);
        ImGui::Text("Press X / Y / Z to change axis");

        int index = 0;
        for (const auto& point : bernsteinPoints)
        {
            bool isSelected = (selectedBernsteinIndex == index);
            std::string label = "B" + std::to_string(index) + "##bernstein" + std::to_string(index);

            if (ImGui::Selectable(label.c_str(), isSelected))
            {
                if (isSelected)
                    selectedBernsteinIndex = -1;
                else
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

void BezierCurveC2::SwapPoints(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint)
{
    for (int i = 0; i < controlPoints.size(); ++i)
    {
        if (auto p = controlPoints[i].lock())
        {
            if (p == oldPoint)
            {
                controlPoints[i] = newPoint;
                newPoint->AddObserver(shared_from_this());
                break;
            }
        }
    }

    UpdateCurve();
}

void BezierCurveC2::OnNotified()
{
    somethingChanged = true;
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
            translation[static_cast<int>(axisMode)] = (axisMode == AxisMode::Y ? normDelta.y : normDelta.x) / 8.f;
            lastTranslation += translation;
	        auto selectedPoint = bernsteinPoints[selectedBernsteinIndex];
            selectedPoint->GetTranslationComponent()->AddTranslation(translation);
            
            RecalculateDeBoorPoints();
        }
    }
}

void BezierCurveC2::RecalculateDeBoorPoints()
{
    if (bernsteinPoints.size() < 4)
    {
        UpdateCurve();
        return;
    }

    int segment = std::max((selectedBernsteinIndex - 1), 0) / 3;

	std::shared_ptr<Point> D0 = controlPoints[segment].lock();
	std::shared_ptr<Point> D1 = controlPoints[segment + 1].lock();
	std::shared_ptr<Point> D2 = controlPoints[segment + 2].lock();
	std::shared_ptr<Point> D3 = controlPoints[segment + 3].lock();

	int modulo = selectedBernsteinIndex % 3;
    
    if (modulo == 0)
    {
        if (selectedBernsteinIndex == 0)
        {
		    D0->GetTranslationComponent()->AddTranslation(6.f * lastTranslation);
        }
        else
        {
		    D3->GetTranslationComponent()->AddTranslation(6.f * lastTranslation);
        }
    }
    else if (modulo == 1)
    {
		D1->GetTranslationComponent()->AddTranslation(2.f * lastTranslation);
		D2->GetTranslationComponent()->AddTranslation(1.f * lastTranslation);
    }
    else if (modulo == 2)
    {
        D1->GetTranslationComponent()->AddTranslation(1.f * lastTranslation);
        D2->GetTranslationComponent()->AddTranslation(2.f * lastTranslation);
    }

    lastTranslation = Algebra::Vector4(0.f, 0.f, 0.f);
    UpdateCurve();
}

void BezierCurveC2::UpdateCurve()
{
	bernsteinPolyline->ClearPoints();
    bernsteinPoints.clear();
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

        for (int j = 0; j < 4; ++j)
        {
			auto p = bezierPoints[j];
            vertices.push_back(PositionVertexData{ .Position = p });
            
            auto point = std::make_shared<Point>();
            point->Init();
            point->GetTranslationComponent()->SetTranslation(p);
            point->SetColor(ColorPalette::Get(Color::Teal));
            if (!(i != 0 && j == 0))
            {
			    bernsteinPoints.push_back(point);
            }

			bernsteinPolyline->AddPoint(point);
        }
    }

    renderer.SetVertices(vertices);
}

json BezierCurveC2::Serialize() const
{
    json j;

    j["objectType"] = "bezierC2";
    j["id"] = static_cast<unsigned int>(id);
    if (!name.empty())
    {
        j["name"] = name;
    }

    json cp = json::array();
    for (const auto& p : controlPoints)
    {
        if (auto point = p.lock())
        {
            cp.push_back(json{ {"id", static_cast<unsigned int>(point->GetId())} });
        }
    }
    j["controlPoints"] = cp;

    return j;
}

std::shared_ptr<BezierCurveC2> BezierCurveC2::Deserialize(const json& j)
{
    auto bc = std::make_shared<BezierCurveC2>();

    bc->id = j.at("id").get<unsigned int>();
    if (j.contains("name"))
    {
        bc->name = j.at("name").get<std::string>();
    }
    else
    {
        bc->name = "Polyline_" + std::to_string(bc->id);
    }

    for (const auto& cp : j.at("controlPoints"))
    {
        unsigned int pid = cp.at("id").get<unsigned int>();
        auto shape = IdManager::GetById(pid);
        if (auto pt = std::dynamic_pointer_cast<Point>(shape))
        {
            bc->AddPoint(pt);
        }
    }

    return bc;
}
