#pragma once

#include "Core/Scene/Entity.h"
#include "Creators/ShapeCreator.h"
#include "ECS/Components/Components.h"
#include "Managers/ShaderManager.h"

#include <numbers>

namespace MeshCreator
{
    // Split these into multiple namespaces

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
        std::vector<ShaderName> shaderNames = { ShaderName::Default })
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
            for (auto shaderName : shaderNames)
            {
				mc.shaders.push_back(ShaderManager::GetInstance().GetShader(shaderName));
            }

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
    MeshData GenerateLineMeshData(const PolylineComponent& lc, Ref<Scene> scene)
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

#pragma region Curve
    MeshData GenerateCurveMeshData(CurveComponent& bcc, Ref<Scene> scene)
    {
        MeshData mesh;
        mesh.vertices = bcc.CallUpdate();
        mesh.indices = {};
        mesh.layout = BufferLayout{
            { ShaderDataType::Float4, "position" }
        };

        return mesh;
    }
#pragma endregion Curve

// points need to be removed upon changes in the deBoor polyline
#pragma region BezierCurveC2
    MeshData GenerateBezierC2MeshData(const BezierCurveC2Component& bcc, Ref<Scene> scene)
    {
        MeshData mesh;

        auto& pointHandles = (Entity{ bcc.deBoorPolylineHandle, scene.get()})
            .GetComponent<PolylineComponent>().pointHandles;

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
                bernsteinPolyline.GetComponent<PolylineComponent>().pointHandles.push_back(pointEntity.GetHandle());
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

#pragma region Patch

    MeshData GeneratePatchMeshData(PatchComponent& pc, Ref<Scene> scene)
    {
        MeshData mesh;
        mesh.vertices = pc.CallUpdate();
        mesh.indices = {};
        mesh.layout = BufferLayout{
            { ShaderDataType::Float4, "position" }
        };

        return mesh;
    }

#pragma endregion Patch
}