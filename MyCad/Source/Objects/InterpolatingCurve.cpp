#include "InterpolatingCurve.h"
#include "Core/App.h"
#include "Algebra.h"
#include "Managers/IdManager.h"

#include <memory>
#include <algorithm>

const float eps = 1e-5f;

InterpolatingCurve::InterpolatingCurve()
    : renderer(VertexDataType::PositionVertexData), 
    bernsteinPolyline(std::make_shared<Polyline>()),
    interpolatingPolyline(std::make_shared<Polyline>())
{
    name = "InterpolatingCurve_" + std::to_string(id);
    bernsteinPolyline->SetColor(ColorPalette::Get(Color::Emerald));
}

void InterpolatingCurve::Render()
{
    if (somethingChanged)
    {
        UpdateCurve();
        somethingChanged = false;
    }

    if (drawBernsteinPolygon)
    {
        bernsteinPolyline->Render();
    }

    if (drawInterpolatingPolygon)
    {
        interpolatingPolyline->Render();
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

    interpolatingPolyline->AddPoint(point);

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

    interpolatingPolyline->RemovePoint(point);

    UpdateCurve();
}

void InterpolatingCurve::SwapPoints(int index1, int index2)
{
    std::swap(controlPoints[index1], controlPoints[index2]);
    interpolatingPolyline->SwapPoints(index1, index2);

    UpdateCurve();
}

void InterpolatingCurve::ReplacePoint(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint)
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

void InterpolatingCurve::OnNotified()
{
    somethingChanged = true;
}

// Move to Algebra library
std::vector<Algebra::Vector4> InterpolatingCurve::SolveTrilinearMatrix(std::vector<float>& alpha, 
    std::vector<float>& beta, std::vector<Algebra::Vector4>& r)
{
    const std::size_t m = r.size();
    std::vector<float> gamma(m);
    std::vector<Algebra::Vector4> delta(m), c(m);

    if (r.size() == 1) 
    {
        c[0] = r[0] / 2.f;
        c[0].w = 0.0f;
        return c;
    }

    float denom = 2.0f;
    gamma[0] = beta[0] / denom;
    delta[0] = r[0] / denom;
    delta[0].w = 0.0f;

    for (std::size_t i = 1; i < m; ++i) 
    {
        denom = 2.0f - alpha[i - 1] * gamma[i - 1];
        if (i < m - 1) 
        {
            gamma[i] = beta[i] / denom;
        }
        Algebra::Vector4 tmp = r[i] - alpha[i - 1] * delta[i - 1];
        delta[i] = tmp / denom;
        delta[i].w = 0.0f;
    }

    c[m - 1] = delta[m - 1];
    c[m - 1].w = 0.0f;
    for (int i = static_cast<int>(m) - 2; i >= 0; --i) 
    {
        Algebra::Vector4 tmp = delta[i] - gamma[i] * c[i + 1];
        c[i] = tmp;
        c[i].w = 0.0f;
    }

    return c;
}

void InterpolatingCurve::UpdateCurve()
{
    bernsteinPolyline->ClearPoints();
    bernsteinPoints.clear();

    controlPoints.erase(
        std::remove_if(controlPoints.begin(), controlPoints.end(),
            [](const std::weak_ptr<Point>& p) {
                return p.expired();
            }),
        controlPoints.end()
    );

    std::vector<Algebra::Vector4> positions;
    for (const auto& p : controlPoints)
    {
        if (auto point = p.lock())
        {
            positions.push_back(point->GetTranslationComponent()->GetTranslation());
        }
    }

    positions.erase(
        std::unique(positions.begin(), positions.end(), 
            [&](auto const& a, auto const& b) { return (a - b).Length() <= eps; }),
        positions.end()
    );

    std::vector<Algebra::Vector4> bezierPoints;

    if (positions.size() <= 1)
    {
        return;
    }
    else if (positions.size() == 2)
    {
        bezierPoints.push_back(positions[0]);
        bezierPoints.push_back(positions[0]);
        bezierPoints.push_back(positions[1]);
        bezierPoints.push_back(positions[1]);

        bezierPoints[0].w = 1.f;
        bezierPoints[1].w = 1.f;
        bezierPoints[2].w = 1.f;
        bezierPoints[3].w = 1.f;
    }
    else
    {
        bezierPoints = CalculateBezierPoints(positions);
    }

    std::vector<PositionVertexData> vertices;
    for (int i = 0; i < bezierPoints.size(); ++i)
    {
        auto p = bezierPoints[i];

        vertices.push_back(PositionVertexData{ .Position = p });

        auto point = std::make_shared<Point>();
        point->Init();
        point->GetTranslationComponent()->SetTranslation(p);
        point->SetColor(ColorPalette::Get(Color::Teal));
        bernsteinPolyline->AddPoint(point);
        bernsteinPoints.push_back(point);
    }

    renderer.SetVertices(vertices);
}

std::vector<Algebra::Vector4> InterpolatingCurve::CalculateBezierPoints(std::vector<Algebra::Vector4>& positions)
{
    std::vector<float> dists;
    std::vector<float> alpha;
    std::vector<float> beta;
    std::vector<Algebra::Vector4> r;

    for (int i = 0; i < positions.size() - 1; i++)
    {
        dists.push_back((positions[i + 1] - positions[i]).Length());
    }

    for (int i = 1; i < dists.size(); i++)
    {
        float d0 = dists[i - 1];
        float d1 = dists[i];
        if (i != 1)
            alpha.push_back(d0 / (d0 + d1));
        if (i != dists.size() - 1)
            beta.push_back(d1 / (d0 + d1));

        Algebra::Vector4 P0 = (positions[i] - positions[i - 1]) / d0;
        Algebra::Vector4 P1 = (positions[i + 1] - positions[i]) / d1;

        r.push_back(3.f * (P1 - P0) / (d0 + d1));
    }

    auto c = SolveTrilinearMatrix(alpha, beta, r);
    c.insert(c.begin(), Algebra::Vector4());
    c.push_back(Algebra::Vector4());
    std::vector<Algebra::Vector4> a(c.size());
    std::vector<Algebra::Vector4> b(c.size());
    std::vector<Algebra::Vector4> d(c.size());

    for (int i = 0; i < dists.size(); i++)
        d[i] = (c[i + 1] - c[i]) / dists[i] / 3.f;

    for (int i = 0; i < a.size(); i++)
        a[i] = positions[i];

    for (int i = 0; i < b.size() - 1; i++)
        b[i] = (a[i + 1] - a[i]) / dists[i] - c[i] * dists[i] - d[i] * dists[i] * dists[i];

    std::vector<Algebra::Vector4> bezierPoints;

    for (int i = 0; i < dists.size(); ++i)
    {
        Algebra::Vector4 ai = a[i];
        Algebra::Vector4 bi = b[i] * dists[i];
        Algebra::Vector4 ci = c[i] * dists[i] * dists[i];
        Algebra::Vector4 di = d[i] * dists[i] * dists[i] * dists[i];

        Algebra::Vector4 P0 = ai;
        Algebra::Vector4 P1 = ai + bi / 3.f;
        Algebra::Vector4 P2 = ai + bi * 2.f / 3.f + ci / 3.f;
        Algebra::Vector4 P3 = ai + bi + ci + di;
        P0.w = P1.w = P2.w = P3.w = 1.0f;

        bezierPoints.push_back(P0);
        bezierPoints.push_back(P1);
        bezierPoints.push_back(P2);
        bezierPoints.push_back(P3);
    }

    return bezierPoints;
}

json InterpolatingCurve::Serialize() const
{
    json j;

    j["objectType"] = "interpolatedC2";
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

std::shared_ptr<InterpolatingCurve> InterpolatingCurve::Deserialize(const json& j)
{
    auto ic = std::make_shared<InterpolatingCurve>();

    ic->id = j.at("id").get<unsigned int>();
    if (j.contains("name"))
    {
        ic->name = j.at("name").get<std::string>();
    }
    else
    {
        ic->name = "Polyline_" + std::to_string(ic->id);
    }

    for (const auto& cp : j.at("controlPoints"))
    {
        unsigned int pid = cp.at("id").get<unsigned int>();
        auto shape = IdManager::GetById(pid);
        if (auto pt = std::dynamic_pointer_cast<Point>(shape))
        {
            ic->AddPoint(pt);
        }
    }

    return ic;
}
