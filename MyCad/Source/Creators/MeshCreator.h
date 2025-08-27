#pragma once

#include "Core/Scene/Entity.h"
#include "Creators/ShapeCreator.h"
#include "ECS/Components/Components.h"
#include "Managers/ShaderManager.h"

#include <numbers>

namespace MeshCreator
{
    struct MeshData
    {
        std::vector<Algebra::Vector4> vertices;
        std::vector<uint32_t> indices;
		BufferLayout layout;

        MeshData() = default;

        MeshData(const MeshData& enemy)
            : vertices(enemy.vertices), indices(enemy.indices), layout(enemy.layout) 
        {
        }

        MeshData(MeshData&& enemy) noexcept
            : vertices(std::move(enemy.vertices)),
            indices(std::move(enemy.indices)),
            layout(std::move(enemy.layout))
        {
        }

        MeshData& operator=(const MeshData& enemy)
        {
            if (this != &enemy)
            {
                vertices = enemy.vertices;
                indices = enemy.indices;
            }
            return *this;
        }

        MeshData& operator=(MeshData&& enemy) noexcept
        {
            if (this != &enemy)
            {
                vertices = std::move(enemy.vertices);
                indices = std::move(enemy.indices);
                layout = std::move(enemy.layout);
            }
            return *this;
        }
    };

    void UpdateMesh(
        Entity& e,
        std::vector<Algebra::Vector4>& vertices,
        std::vector<uint32_t>& indices,
        BufferLayout layout = BufferLayout{ { ShaderDataType::Float4, "position" } },
        RenderingMode mode = RenderingMode::Lines,
        ShaderName shaderName = ShaderName::Default)
    {
        if (!e.HasComponent<MeshComponent>())
        {
            Ref<VertexBuffer> vb = CreateRef<VertexBuffer>(
                vertices.data(),
                static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4))
            );
            vb->SetLayout(layout);

            Ref<IndexBuffer> ib = indices.empty() ? nullptr 
                : CreateRef<IndexBuffer>(indices.data(), indices.size());

            auto va = CreateRef<VertexArray>();
            va->AddVertexBuffer(vb);
            if (ib)
            {
                va->SetIndexBuffer(ib);
            }

            auto& mc = e.EmplaceComponent<MeshComponent>();
            mc.vertexArray = va;
            mc.renderingMode = mode;
            mc.shader = ShaderManager::GetInstance().GetShader(shaderName);

            va->Unbind();
        }
        else
        {
            auto& mc = e.GetComponent<MeshComponent>();
            auto vb = mc.vertexArray->GetVertexBuffers()[0];
            vb->SetData(vertices.data(),
                static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4)));
            vb->SetLayout(layout);

            if (!indices.empty())
            {
                mc.vertexArray->SetIndexBuffer(CreateRef<IndexBuffer>(indices.data(), indices.size()));
            }
        }
    }

#pragma region Point
    MeshData GeneratePointMeshData()
    {
        static std::vector<Algebra::Vector4> vertices = {
            { Algebra::Vector4(-0.05f, -0.05f, -0.05f, 1.0f) },
            { Algebra::Vector4( 0.05f, -0.05f, -0.05f, 1.0f) },
            { Algebra::Vector4( 0.05f,  0.05f, -0.05f, 1.0f) },
            { Algebra::Vector4(-0.05f,  0.05f, -0.05f, 1.0f) },
            { Algebra::Vector4(-0.05f, -0.05f,  0.05f, 1.0f) },
            { Algebra::Vector4( 0.05f, -0.05f,  0.05f, 1.0f) },
            { Algebra::Vector4( 0.05f,  0.05f,  0.05f, 1.0f) },
            { Algebra::Vector4(-0.05f,  0.05f,  0.05f, 1.0f) },
        };

        static std::vector<uint32_t> indices = {
            0, 1, 2,  2, 3, 0,
            4, 5, 6,  6, 7, 4,
            0, 3, 7,  7, 4, 0,
            1, 5, 6,  6, 2, 1,
            0, 1, 5,  5, 4, 0,
            3, 2, 6,  6, 7, 3
        };

        MeshData mesh;
        mesh.vertices = vertices;
        mesh.indices = indices;
        mesh.layout = BufferLayout{
            { ShaderDataType::Float4, "position" }
        };

        return mesh;
    }
#pragma endregion Point

#pragma region Torus
    Algebra::Vector4 GetTorusPoint(float angleTube, float angleRadius, float radius, float tubeRadius)
    {
        return Algebra::Matrix4::RotationY(angleRadius) *
            Algebra::Vector4(radius + tubeRadius * cosf(angleTube), tubeRadius * sinf(angleTube), 0.f, 1.f);
    }

    MeshData GenerateTorusMeshData(const TorusComponent& tc)
    {
        MeshData mesh;

        for (int i = 0; i < tc.majorSegments; i++)
        {
            for (int j = 0; j < tc.minorSegments; j++)
            {
                Algebra::Vector4 vertex = GetTorusPoint(
                    2 * std::numbers::pi_v<float> *j / tc.minorSegments,
                    2 * std::numbers::pi_v<float> *i / tc.majorSegments,
                    tc.majorRadius, tc.minorRadius
                );
                mesh.vertices.push_back(vertex);
            }
        }

        for (int i = 0; i < tc.majorSegments; i++)
        {
            int iNext = (i + 1) % tc.majorSegments;
            for (int j = 0; j < tc.minorSegments; j++)
            {
                int jNext = (j + 1) % tc.minorSegments;
                mesh.indices.push_back(tc.minorSegments * i + j);
                mesh.indices.push_back(tc.minorSegments * i + jNext);
                mesh.indices.push_back(tc.minorSegments * i + j);
                mesh.indices.push_back((tc.minorSegments * iNext) + j);
            }
        }

		mesh.layout = BufferLayout {
            { ShaderDataType::Float4, "position" }
        };

        return mesh;
    }
#pragma endregion Torus

#pragma region Line
    MeshData GenerateLineMeshData(const LineComponent& lc, Ref<Scene> scene)
    {
        MeshData mesh;

        for (auto handle : lc.pointHandles)
        {
            Entity pointEntity{ handle, scene.get() };

            if (pointEntity.HasComponent<TranslationComponent>())
            {
                Algebra::Vector4 vertex = pointEntity.GetComponent<TranslationComponent>().translation;
                vertex.w = 1.0f;
                mesh.vertices.push_back(vertex);
            }
        }

        for (size_t i = 0; i + 1 < lc.pointHandles.size(); ++i)
        {
            mesh.indices.push_back(static_cast<uint32_t>(i));
            mesh.indices.push_back(static_cast<uint32_t>(i + 1));
        }

        mesh.layout = BufferLayout{
            { ShaderDataType::Float4, "position" }
        };

        return mesh;
    }
#pragma endregion Line

#pragma region BezierCurveC0
    void PadBezierVertices(std::vector<Algebra::Vector4>& vertices)
    {
        int rest = static_cast<int>(vertices.size() % 4);
        if (rest == 0) return;

        switch (rest)
        {
        case 1:
        {
            auto p = vertices.back();
            vertices.insert(vertices.end(), 3, p);
            break;
        }
        case 2:
        {
            auto p0 = vertices[vertices.size() - 2];
            auto p1 = vertices.back();
            vertices.pop_back();
            vertices.pop_back();
            vertices.push_back(p0);
            vertices.push_back(p0);
            vertices.push_back(p1);
            vertices.push_back(p1);
            break;
        }
        case 3:
        {
            auto p0 = vertices[vertices.size() - 3];
            auto p1 = vertices[vertices.size() - 2];
            auto p2 = vertices.back();
            vertices.pop_back();
            vertices.pop_back();
            vertices.pop_back();
            vertices.push_back(p0);
            vertices.push_back((1.f / 3.f) * p0 + (2.f / 3.f) * p1);
            vertices.push_back((2.f / 3.f) * p1 + (1.f / 3.f) * p2);
            vertices.push_back(p2);
            break;
        }
        }
    }

    MeshData GenerateBezierC0MeshData(const BezierCurveC0Component& bcc, Ref<Scene> scene)
    {
        MeshData mesh;

        auto& pointHandles = (Entity{ bcc.polylineHandle, scene.get()}).GetComponent<LineComponent>().pointHandles;

        int i = 0;
        for (auto handle : pointHandles)
        {
            Entity pointEntity{ handle, scene.get()};

            if (pointEntity.HasComponent<TranslationComponent>())
            {
                Algebra::Vector4 vertex = pointEntity.GetComponent<TranslationComponent>().translation;
                vertex.w = 1.0f;
                mesh.vertices.push_back(vertex);
                ++i;

                if (i % 4 == 0)
                {
                    // duplicate every 4th point
                    mesh.vertices.push_back(vertex);
                    ++i;
                }
            }
        }

		PadBezierVertices(mesh.vertices);

        mesh.layout = BufferLayout{
            { ShaderDataType::Float4, "position" }
        };

        return mesh;
    }
#pragma endregion BezierCurveC0

#pragma region BezierCurveC2
    MeshData GenerateBezierC2MeshData(const BezierCurveC2Component& bcc, Ref<Scene> scene)
    {
        MeshData mesh;

        auto& pointHandles = (Entity{ bcc.deBoorPolylineHandle, scene.get()})
            .GetComponent<LineComponent>().pointHandles;

        if (pointHandles.size() < 4)
            return mesh;

        Entity bernsteinPolyline{ bcc.bernsteinPolylineHandle, scene.get() };

        for (size_t i = 0; i + 3 < pointHandles.size(); ++i)
        {
            auto d0 = Entity{ pointHandles[i]    , scene.get() };
            auto d1 = Entity{ pointHandles[i + 1], scene.get() };
            auto d2 = Entity{ pointHandles[i + 2], scene.get() };
            auto d3 = Entity{ pointHandles[i + 3], scene.get() };

            Algebra::Vector4 D0 = d0.GetComponent<TranslationComponent>().translation;
            Algebra::Vector4 D1 = d1.GetComponent<TranslationComponent>().translation;
            Algebra::Vector4 D2 = d2.GetComponent<TranslationComponent>().translation;
            Algebra::Vector4 D3 = d3.GetComponent<TranslationComponent>().translation;

            D0.w = D1.w = D2.w = D3.w = 1.0f;

            Algebra::Vector4 bezierPoints[4] = {
                (D0 + 4.0f * D1 + D2) / 6.0f,
                (2.0f * D1 + D2) / 3.0f,
                (D1 + 2.0f * D2) / 3.0f,
                (D1 + 4.0f * D2 + D3) / 6.0f
            };

            for (int j = 0; j < 4; ++j)
            {
                auto pointEntity = ShapeCreator::CreatePoint(scene);
                pointEntity.EmplaceTag<IsInvisibleTag>();
                pointEntity.GetComponent<TranslationComponent>().SetTranslation(bezierPoints[j]);
                bernsteinPolyline.GetComponent<LineComponent>().pointHandles.push_back(pointEntity.GetHandle());
                pointEntity.EmplaceComponent<VirtualComponent>(bernsteinPolyline.GetHandle());

                mesh.vertices.push_back(bezierPoints[j]);
            }
        }

        mesh.layout = BufferLayout{
            { ShaderDataType::Float4, "position" }
        };

        return mesh;
    }

#pragma endregion BezierCurveC2

#pragma region InterpolatingCurve
    std::vector<Algebra::Vector4> SolveTrilinearMatrix(std::vector<float>& alpha,
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
    
    std::vector<Algebra::Vector4> CalculateBernsteinPoints(std::vector<Algebra::Vector4>& positions)
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

    // Needs the adjustment for if the points are too close to each other
    MeshData GenerateInterpolatingCurveMeshData(const InterpolatingCurveComponent& icc, Ref<Scene> scene)
    {
        MeshData mesh;

        auto& pointHandles = (Entity{ icc.interpolatingPolylineHandle, scene.get() })
            .GetComponent<LineComponent>().pointHandles;

		std::vector<Algebra::Vector4> positions;
        for (auto pointHandle : pointHandles)
        {
            Entity p{ pointHandle, scene.get() };
			positions.push_back(p.GetComponent<TranslationComponent>().translation);
        }

        std::vector<Algebra::Vector4> bernsteinPoints;

        if (positions.size() <= 1)
        {
            return mesh;
        }
        else if (positions.size() == 2)
        {
            bernsteinPoints.push_back(positions[0]);
            bernsteinPoints.push_back(positions[0]);
            bernsteinPoints.push_back(positions[1]);
            bernsteinPoints.push_back(positions[1]);

            bernsteinPoints[0].w = 1.f;
            bernsteinPoints[1].w = 1.f;
            bernsteinPoints[2].w = 1.f;
            bernsteinPoints[3].w = 1.f;
        }
        else
        {
            bernsteinPoints = CalculateBernsteinPoints(positions);
        }

        Entity bernsteinPolyline{ icc.bernsteinPolylineHandle, scene.get() };

        for (size_t i = 0; i < bernsteinPoints.size(); ++i)
        {
            auto pointEntity = ShapeCreator::CreatePoint(scene);
            pointEntity.EmplaceTag<IsInvisibleTag>();
            pointEntity.GetComponent<TranslationComponent>().SetTranslation(bernsteinPoints[i]);
            bernsteinPolyline.GetComponent<LineComponent>().pointHandles.push_back(pointEntity.GetHandle());
            pointEntity.EmplaceComponent<VirtualComponent>(bernsteinPolyline.GetHandle());

            mesh.vertices.push_back(bernsteinPoints[i]);
        }

        mesh.layout = BufferLayout{
            { ShaderDataType::Float4, "position" }
        };

        return mesh;
    }

#pragma endregion InterpolatingCurve

#pragma region BezierSurfaceC0

    std::vector<Algebra::Vector4> SetupControlPoints(Algebra::Vector4 position, float width, float height,
        int rows, int columns)
    {
        std::vector<Algebra::Vector4> controlPoints;
        controlPoints.reserve(rows * columns);

        float dx = width / static_cast<float>(columns - 1);
        float dy = height / static_cast<float>(rows - 1);

        Algebra::Vector4 startingPosition = position - Algebra::Vector4(width / 2.f, height / 2.f, 0.f);

        for (int i = 0; i < rows; ++i)
        {
            for (int j = 0; j < columns; ++j)
            {
                auto heightOffset = Algebra::Vector4(0.f, i * dy, 0.f);
                auto widthOffset = Algebra::Vector4(j * dx, 0.f, 0.f);

                controlPoints.push_back(startingPosition + widthOffset + heightOffset);
            }
        }

        return controlPoints;
    }

    std::vector<Algebra::Vector4> SetupPatches(const BezierSurfaceC0Component& bsc, Ref<Scene> scene,
		const std::vector<Algebra::Vector4>& controlPoints, int rows, int columns)
    {
        std::vector<Algebra::Vector4> patchPoints;
        for (int patchIndex = 0; patchIndex < bsc.widthPatches * bsc.heightPatches; ++patchIndex)
        {
            int startingI = patchIndex / bsc.widthPatches;
            int startingJ = patchIndex % bsc.widthPatches;

            for (int i = 0; i < 4; ++i)
            {
                for (int j = 0; j < 4; ++j)
                {
                    patchPoints.push_back(controlPoints[(startingI * 3 + i) * columns + startingJ * 3 + j]);
                }
            }
        }

        return patchPoints;
    }

    MeshData GenerateBezierSurfaceC0MeshData(BezierSurfaceC0Component& bsc, Ref<Scene> scene)
    {
		int rows = bsc.heightPatches * 3 + 1;
		int columns = bsc.widthPatches * 3 + 1;

		auto controlPoints = SetupControlPoints(Algebra::Vector4(0.f, 0.f, 0.f, 1.f), 2.f, 2.f, rows, columns);
        
        for(auto& p : controlPoints)
        {
            p.w = 1.f;
		}

		auto vertices = SetupPatches(bsc, scene, controlPoints, rows, columns);

        MeshData mesh;
        mesh.vertices = vertices;
        mesh.indices = {};
        mesh.layout = BufferLayout{
            { ShaderDataType::Float4, "position" }
        };

        return mesh;
    }

#pragma endregion BezierSurfaceC0
}