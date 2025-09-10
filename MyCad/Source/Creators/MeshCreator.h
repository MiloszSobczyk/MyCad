#pragma once

#include "Core/Scene/Entity.h"
#include "Creators/ShapeCreator.h"
#include "ECS/Components/Components.h"
#include "Managers/ShaderManager.h"
#include "Managers/UniformManager.h"

#include <numbers>

using namespace Surface::Gregory;

namespace MeshCreator
{
    struct MeshData
    {
        std::vector<Algebra::Vector4> vertices;
        std::vector<uint32_t> indices;
		BufferLayout layout;
        Ref<UniformCalculation> uniformCalculation;

        MeshData() = default;

        MeshData(const MeshData& enemy)
            : vertices(enemy.vertices), 
            indices(enemy.indices), 
            layout(enemy.layout),
            uniformCalculation(uniformCalculation)
        {
        }

        MeshData(MeshData&& enemy) noexcept
            : vertices(std::move(enemy.vertices)),
            indices(std::move(enemy.indices)),
            layout(std::move(enemy.layout)),
            uniformCalculation(std::move(enemy.uniformCalculation))
        {
        }

        MeshData& operator=(const MeshData& enemy)
        {
            if (this != &enemy)
            {
                vertices = enemy.vertices;
                indices = enemy.indices;
                layout = enemy.layout;
                uniformCalculation = enemy.uniformCalculation;
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
                uniformCalculation = std::move(enemy.uniformCalculation);
            }
            return *this;
        }
    };

    void UpdateMesh(
        Entity& e,
        std::vector<Algebra::Vector4>& vertices,
        std::vector<uint32_t>& indices,
        Ref<UniformCalculation> uniformCalculation,
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
            mc.uniformCalculation = uniformCalculation;
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

    namespace Point
    {
        MeshData GenerateMeshData()
        {
            static std::vector<Algebra::Vector4> vertices = {
                { Algebra::Vector4(-0.03f, -0.03f, -0.03f, 1.0f) },
                { Algebra::Vector4( 0.03f, -0.03f, -0.03f, 1.0f) },
                { Algebra::Vector4( 0.03f,  0.03f, -0.03f, 1.0f) },
                { Algebra::Vector4(-0.03f,  0.03f, -0.03f, 1.0f) },
                { Algebra::Vector4(-0.03f, -0.03f,  0.03f, 1.0f) },
                { Algebra::Vector4( 0.03f, -0.03f,  0.03f, 1.0f) },
                { Algebra::Vector4( 0.03f,  0.03f,  0.03f, 1.0f) },
                { Algebra::Vector4(-0.03f,  0.03f,  0.03f, 1.0f) },
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
            mesh.uniformCalculation = UniformManager::GetInstance().GetCalculation<PointComponent>();

            return mesh;
        }
    }

    namespace Torus
    {
        Algebra::Vector4 CalculatePointPosition(float angleTube, float angleRadius, float radius, float tubeRadius)
        {
            return Algebra::Matrix4::RotationY(angleRadius) *
                Algebra::Vector4(radius + tubeRadius * cosf(angleTube), tubeRadius * sinf(angleTube), 0.f, 1.f);
        }

        MeshData GenerateMeshData(const TorusComponent& tc)
        {
            MeshData mesh;

            for (int i = 0; i < tc.majorSegments; i++)
            {
                for (int j = 0; j < tc.minorSegments; j++)
                {
                    Algebra::Vector4 vertex = CalculatePointPosition(
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
            mesh.uniformCalculation = UniformManager::GetInstance().GetCalculation<TorusComponent>();

            return mesh;
        }
    }

    namespace Polyline
    {
        MeshData GenerateMeshData(const PolylineComponent& lc, Ref<Scene> scene)
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
            mesh.uniformCalculation = UniformManager::GetInstance().GetCalculation<PolylineComponent>();

            return mesh;
        }
    }

    namespace Curve
    {
        MeshData GenerateMeshData(CurveComponent& bcc, Ref<Scene> scene)
        {
            MeshData mesh;
            mesh.vertices = bcc.CallUpdate();
            mesh.indices = {};
            mesh.layout = BufferLayout{
                { ShaderDataType::Float4, "position" }
            };
            mesh.uniformCalculation = UniformManager::GetInstance().GetCalculation<CurveComponent>();

            return mesh;
        }
    }

    namespace Patch
    {
        MeshData GenerateMeshData(PatchComponent& pc, Ref<Scene> scene)
        {
            MeshData mesh;
            mesh.vertices = pc.CallUpdate();
            mesh.indices = {};
            mesh.layout = BufferLayout{
                { ShaderDataType::Float4, "position" }
            };
            mesh.uniformCalculation = UniformManager::GetInstance().GetCalculation<PatchComponent>();

            return mesh;
        }
    }

    namespace GregoryPatch
    {
        MeshData GenerateGregoryPoints(GregoryPatchComponent& gpc, Ref<Scene> scene)
        {
            MeshData mesh;

            auto triangle = FindCycles(scene);

            PrintTriangle(triangle, scene);

            return mesh;
        }

        MeshData GenerateMeshData(GregoryPatchComponent& gpc, Ref<Scene> scene)
        {
            return GenerateGregoryPoints(gpc, scene);
        }
    }
}