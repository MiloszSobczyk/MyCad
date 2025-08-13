#include "MeshGenerationSystem.h"

#include "Core/Scene/Entity.h"
#include "ECS/Components/MeshComponent.h"
#include "ECS/Components/TorusComponent.h"
#include "ECS/Tags/IsDirtyTag.h"

#include "Algebra.h"

#include <numbers>


MeshGenerationSystem::MeshGenerationSystem(Ref<Scene> scene)
	: m_Scene(scene)
{
}

void MeshGenerationSystem::Update()
{
	GenerateTorusMesh();
}

Algebra::Vector4 GetPoint2(float angleTube, float angleRadius, float radius, float tubeRadius)
{
	return Algebra::Matrix4::RotationY(angleRadius) *
		Algebra::Vector4(radius + tubeRadius * cosf(angleTube), tubeRadius * sinf(angleTube), 0.f, 1.f);
}

void MeshGenerationSystem::GenerateTorusMesh()
{
	for (auto entity : m_Scene->GetAllEntitiesWith<IsDirtyTag, TorusComponent>())
	{
		Entity e{ entity, m_Scene.get() };

		e.RemoveComponent<IsDirtyTag>();

		const auto& tc = e.GetComponent<TorusComponent>();

		std::vector<Algebra::Vector4> vertices;
		std::vector<uint32_t> indices;

		for (int i = 0; i < tc.majorSegments; i++)
		{
			for (int j = 0; j < tc.minorSegments; j++)
			{
				Algebra::Vector4 vertex;
				vertex = GetPoint2(2 * std::numbers::pi_v<float> * j / tc.minorSegments,
					2 * std::numbers::pi_v<float> * i / tc.majorSegments, tc.majorRadius, tc.minorRadius);
				vertices.push_back(vertex);
			}
		}

		for (int i = 0; i < tc.majorSegments; i++)
		{
			int iNext = (i + 1) % tc.majorSegments;
			for (int j = 0; j < tc.minorSegments; j++)
			{
				int jNext = (j + 1) % tc.minorSegments;
				indices.push_back(tc.minorSegments * i + j);
				indices.push_back(tc.minorSegments * i + jNext);
				indices.push_back(tc.minorSegments * i + j);
				indices.push_back((tc.minorSegments * iNext) + j);
			}
		}

		BufferLayout layout = {
			{ ShaderDataType::Float4, "position" }
		};

		if (!e.HasComponent<MeshComponent>())
		{
			Ref<VertexBuffer> vb = CreateRef<VertexBuffer>(vertices.data(),
				static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4)));
			vb->SetLayout(layout);

			Ref<IndexBuffer> ib = CreateRef<IndexBuffer>(indices.data(), indices.size());

			auto va = CreateRef<VertexArray>();
			va->AddVertexBuffer(vb);
			va->SetIndexBuffer(ib);

			auto mc = &e.EmplaceComponent<MeshComponent>();
			mc->vertexArray = va;
		}
		else
		{
			auto mc = &e.GetComponent<MeshComponent>();
			mc->vertexArray->GetVertexBuffers()[0]->SetData(vertices.data(),
				static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4)));
			mc->vertexArray->GetVertexBuffers()[0]->SetLayout(layout);
			mc->vertexArray->SetIndexBuffer(CreateRef<IndexBuffer>(indices.data(), indices.size()));
		}
	}
}
