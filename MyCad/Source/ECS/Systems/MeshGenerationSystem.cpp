#include "MeshGenerationSystem.h"

#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"
#include "Managers/ShaderManager.h"

#include "Algebra.h"

#include <numbers>


MeshGenerationSystem::MeshGenerationSystem(Ref<Scene> scene)
	: m_Scene{ scene }
{
}

void MeshGenerationSystem::Update()
{
	UpdateTorusMeshes();
	UpdatePointMeshes();
	UpdatePolylineMeshes();
}

Algebra::Vector4 GetPoint(float angleTube, float angleRadius, float radius, float tubeRadius)
{
	return Algebra::Matrix4::RotationY(angleRadius) *
		Algebra::Vector4(radius + tubeRadius * cosf(angleTube), tubeRadius * sinf(angleTube), 0.f, 1.f);
}

void MeshGenerationSystem::UpdateTorusMeshes()
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
				vertex = GetPoint(2 * std::numbers::pi_v<float> * j / tc.minorSegments,
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
			mc->renderingMode = RenderingMode::Lines;
			mc->shader = ShaderManager::GetInstance().GetShader(ShaderName::Default);

			va->Unbind();
		}
		else
		{
			// possible source of bugs but have no way of testing it right now
			auto mc = &e.GetComponent<MeshComponent>();
			mc->vertexArray->GetVertexBuffers()[0]->SetData(vertices.data(),
				static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4)));
			mc->vertexArray->GetVertexBuffers()[0]->SetLayout(layout);
			mc->vertexArray->SetIndexBuffer(CreateRef<IndexBuffer>(indices.data(), indices.size()));
		}
	}
}

void MeshGenerationSystem::UpdatePointMeshes()
{
	static std::vector<Algebra::Vector4> vertices = {
		{ Algebra::Vector4(-0.1f, -0.1f, -0.1f, 1.0f) },
		{ Algebra::Vector4( 0.1f, -0.1f, -0.1f, 1.0f) },
		{ Algebra::Vector4( 0.1f,  0.1f, -0.1f, 1.0f) },
		{ Algebra::Vector4(-0.1f,  0.1f, -0.1f, 1.0f) },
		{ Algebra::Vector4(-0.1f, -0.1f,  0.1f, 1.0f) },
		{ Algebra::Vector4( 0.1f, -0.1f,  0.1f, 1.0f) },
		{ Algebra::Vector4( 0.1f,  0.1f,  0.1f, 1.0f) },
		{ Algebra::Vector4(-0.1f,  0.1f,  0.1f, 1.0f) },
	};

	static std::vector<uint32_t> indices = {
		0, 1, 2,  2, 3, 0,
		4, 5, 6,  6, 7, 4,
		0, 3, 7,  7, 4, 0,
		1, 5, 6,  6, 2, 1,
		0, 1, 5,  5, 4, 0,
		3, 2, 6,  6, 7, 3
	};

	for (auto entity : m_Scene->GetAllEntitiesWith<IsDirtyTag, PointComponent>())
	{
		Entity e{ entity, m_Scene.get() };
		e.RemoveComponent<IsDirtyTag>();

		if (e.HasComponent<MeshComponent>())
			continue;

		BufferLayout layout = {
			{ ShaderDataType::Float4, "position" }
		};

		Ref<VertexBuffer> vb = CreateRef<VertexBuffer>(vertices.data(),
			static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4)));
		vb->SetLayout(layout);

		Ref<IndexBuffer> ib = CreateRef<IndexBuffer>(indices.data(), indices.size());

		auto va = CreateRef<VertexArray>();
		va->AddVertexBuffer(vb);
		va->SetIndexBuffer(ib);

		auto mc = &e.EmplaceComponent<MeshComponent>();
		mc->vertexArray = va;
		mc->renderingMode = RenderingMode::Triangles;
		mc->shader = ShaderManager::GetInstance().GetShader(ShaderName::Default);

		va->Unbind();
	}
}

void MeshGenerationSystem::UpdatePolylineMeshes()
{
	for (auto entity : m_Scene->GetAllEntitiesWith<IsDirtyTag, PolylineComponent>())
	{
		Entity e{ entity, m_Scene.get() };

		e.RemoveComponent<IsDirtyTag>();

		const auto& pc = e.GetComponent<PolylineComponent>();

		std::vector<Algebra::Vector4> vertices;
		std::vector<uint32_t> indices;

		for (auto point : pc.pointHandles)
		{
			Entity pointEntity{ point, m_Scene.get() };

			if (pointEntity.HasComponent<TranslationComponent>())
			{
				Algebra::Vector4 vertex = pointEntity.GetComponent<TranslationComponent>().translation;
				vertex.w = 1.0f;
				vertices.push_back(vertex);
			}
		}

		for (int i = 0; i < pc.pointHandles.size() - 1; ++i)
		{
			indices.push_back(i);
			indices.push_back(i + 1);
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
			mc->renderingMode = RenderingMode::Lines;
			mc->shader = ShaderManager::GetInstance().GetShader(ShaderName::Default);

			va->Unbind();
		}
		else
		{
			// possible source of bugs but have no way of testing it right now
			auto mc = &e.GetComponent<MeshComponent>();
			mc->vertexArray->GetVertexBuffers()[0]->SetData(vertices.data(),
				static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4)));
			mc->vertexArray->GetVertexBuffers()[0]->SetLayout(layout);
			mc->vertexArray->SetIndexBuffer(CreateRef<IndexBuffer>(indices.data(), indices.size()));
		}
	}
}
