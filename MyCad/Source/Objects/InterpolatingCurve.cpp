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

std::vector<Algebra::Vector4> InterpolatingCurve::SolveTrilinearMatrix(std::vector<float>& alpha, std::vector<float>& beta, std::vector<Algebra::Vector4>& R)
{
    const std::size_t m = R.size();
    std::vector<float> gamma(m);
    std::vector<Algebra::Vector4> delta(m), c(m);

    float denom = 2.0f;
    gamma[0] = beta[0] / denom;
    delta[0] = R[0] / denom;
    delta[0].w = 0.0f;

    for (std::size_t i = 1; i < m; ++i) {
        denom = 2.0f - alpha[i - 1] * gamma[i - 1];
        if (i < m - 1) {
            gamma[i] = beta[i] / denom;
        }
        Algebra::Vector4 tmp = R[i] - alpha[i - 1] * delta[i - 1];
        delta[i] = tmp / denom;
        delta[i].w = 0.0f;
    }

    c[m - 1] = delta[m - 1];
    c[m - 1].w = 0.0f;
    for (int i = static_cast<int>(m) - 2; i >= 0; --i) {
        Algebra::Vector4 tmp = delta[i] - gamma[i] * c[i + 1];
        c[i] = tmp;
        c[i].w = 0.0f;
    }

    return c;
}


void InterpolatingCurve::Calculate()
{
    // Must have at least 4 control points for C2 spline
    if (controlPoints.size() < 3)
        return;

    // 1) Gather 3D positions
    std::vector<Algebra::Vector4> positions;
    positions.reserve(controlPoints.size());
    for (auto& wp : controlPoints)
        if (auto p = wp.lock())
            positions.push_back(p->GetTranslationComponent()->GetTranslation());

    size_t N = positions.size();
    size_t m = N - 1; // number of spline segments

    // 2) Compute chord-length distances h_i
    std::vector<float> h(m);
    for (size_t i = 0; i < m; ++i)
        h[i] = (positions[i + 1] - positions[i]).Length();

    // 3) Build tridiagonal system parameters alpha, beta and RHS r
    std::vector<float> alpha(m - 1), beta(m - 1);
    std::vector<Algebra::Vector4> r(m - 1);
    for (size_t i = 1; i < m; ++i) {
        float h0 = h[i - 1];
        float h1 = h[i];
        alpha[i - 1] = h0 / (h0 + h1);
        beta[i - 1] = h1 / (h0 + h1);
        Algebra::Vector4 d0 = (positions[i] - positions[i - 1]) / h0;
        Algebra::Vector4 d1 = (positions[i + 1] - positions[i]) / h1;
        r[i - 1] = 3.0f * (d1 - d0) / (h0 + h1);
        r[i - 1].w = 0.0f;
    }

    // 4) Solve internal second derivatives
    auto c_internal = SolveTrilinearMatrix(alpha, beta, r);

    // 5) Assemble full second-derivatives with natural boundary
    std::vector<Algebra::Vector4> c(N);
    c[0] = Algebra::Vector4(0, 0, 0, 0);
    for (size_t i = 1; i < N - 1; ++i)
        c[i] = c_internal[i - 1];
    c[N - 1] = Algebra::Vector4(0, 0, 0, 0);

    // 6) Compute first-derivatives
    std::vector<Algebra::Vector4> b(N);
    for (size_t i = 0; i < m; ++i) {
        // Q'(t_i)
        b[i] = (positions[i + 1] - positions[i]) / h[i]
            - (h[i] * (2.0f * c[i] + c[i + 1])) / 6.0f;
    }

    // 7) Build Bézier control points per segment and feed renderer
    std::vector<PositionVertexData> vertices;
    vertices.reserve(m * 4);

    for (size_t i = 0; i < m; ++i) {
        float hi = h[i];
        // Convert to Bernstein control points
        Algebra::Vector4 P0 = positions[i];
        Algebra::Vector4 P1 = P0 + b[i] * (hi / 3.0f);
        Algebra::Vector4 P2 = P0 + b[i] * (2.0f * hi / 3.0f)
            + c[i] * (hi * hi / 6.0f);
        Algebra::Vector4 P3 = positions[i + 1];
        P0.w = P1.w = P2.w = P3.w = 1.0f;

        // Push control points for GPU tessellation
        vertices.push_back(PositionVertexData{ .Position = P0 });
        vertices.push_back(PositionVertexData{ .Position = P1 });
        vertices.push_back(PositionVertexData{ .Position = P2 });
        vertices.push_back(PositionVertexData{ .Position = P3 });
    }

    renderer.SetVertices(vertices);
}


void InterpolatingCurve::UpdateCurve()
{
    //bernsteinPolyline->ClearPoints();

    //if (controlPoints.size() < 4)
    //    return;

    //controlPoints.erase(
    //    std::remove_if(controlPoints.begin(), controlPoints.end(),
    //        [](const std::weak_ptr<Point>& p) {
    //            return p.expired();
    //        }),
    //    controlPoints.end()
    //);
}
