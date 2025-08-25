#include "MeshGenerationSystem.h"

#include "Core/Scene/Entity.h"
#include "Creators/ShapeCreator.h"
#include "ECS/Components/Components.h"
#include "Managers/ShaderManager.h"

#include "Algebra.h"

#include <numbers>

// Function for quick vertex array creation is needed

MeshGenerationSystem::MeshGenerationSystem(Ref<Scene> scene)
	: m_Scene{ scene }
{
}

void MeshGenerationSystem::Update()
{
	UpdateDirtyTags();
	UpdateTorusMeshes();
	UpdatePointMeshes();
	UpdateLineMeshes();
}

Algebra::Vector4 GetPoint(float angleTube, float angleRadius, float radius, float tubeRadius)
{
	return Algebra::Matrix4::RotationY(angleRadius) *
		Algebra::Vector4(radius + tubeRadius * cosf(angleTube), tubeRadius * sinf(angleTube), 0.f, 1.f);
}

void MeshGenerationSystem::UpdateDirtyTags()
{
	for (auto e : m_Scene->GetAllEntitiesWith<VirtualComponent, IsDirtyTag>())
	{
		auto& vc = e.GetComponent<VirtualComponent>();
		Entity targetEntity{ vc.targetEntity, m_Scene.get() };
		targetEntity.EmplaceTag<IsDirtyTag>();
	}
}

void MeshGenerationSystem::UpdateTorusMeshes()
{
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, TorusComponent>())
	{
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

	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, PointComponent>())
	{
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

// Removing point from mesh should also remove mesh's reference from point
void MeshGenerationSystem::UpdateLineMeshes()
{
	// Polylines
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, LineComponent>())
	{
		const auto& pc = e.GetComponent<LineComponent>();

		if (pc.pointHandles.empty())
		{
			continue;
		}

		e.RemoveComponent<IsDirtyTag>();
		
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
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, BezierCurveC0Component>())
	{
		e.RemoveComponent<IsDirtyTag>();

		const auto& bcc = e.GetComponent<BezierCurveC0Component>();

		std::vector<Algebra::Vector4> vertices;
		std::vector<uint32_t> indices;

		auto& pointHandles = (Entity{ bcc.polylineHandle, m_Scene.get() }).GetComponent<LineComponent>().pointHandles;

		int i = 0;
		for (auto point : pointHandles)
		{
			Entity pointEntity{ point, m_Scene.get() };

			if (pointEntity.HasComponent<TranslationComponent>())
			{
				Algebra::Vector4 vertex = pointEntity.GetComponent<TranslationComponent>().translation;
				vertex.w = 1.0f;
				vertices.push_back(vertex);
				++i;

				if (i % 4 == 0)
				{
					vertices.push_back(vertex);
					++i;
				}
			}
		}


		int rest = vertices.size() % 4;

		if (rest == 1)
		{
			auto p = vertices[vertices.size() - 1];
			vertices.push_back(p);
			vertices.push_back(p);
			vertices.push_back(p);
		}
		else if (rest == 2)
		{
			auto p0 = vertices[vertices.size() - 2];
			auto p1 = vertices[vertices.size() - 1];
			vertices.pop_back();
			vertices.pop_back();
			vertices.push_back(p0);
			vertices.push_back(p0);
			vertices.push_back(p1);
			vertices.push_back(p1);
		}
		else if (rest == 3)
		{
			auto p0 = vertices[vertices.size() - 3];
			auto p1 = vertices[vertices.size() - 2];
			auto p2 = vertices[vertices.size() - 1];
			vertices.pop_back();
			vertices.pop_back();
			vertices.pop_back();
			vertices.push_back(p0);
			vertices.push_back(Algebra::Vector4{ 1.f / 3.f * p0 + 2.f / 3.f * p1 });
			vertices.push_back(Algebra::Vector4{ 2.f / 3.f * p1 + 1.f / 3.f * p2 });
			vertices.push_back(p2);
		}

		BufferLayout layout = {
			{ ShaderDataType::Float4, "position" }
		};

		if (!e.HasComponent<MeshComponent>())
		{
			Ref<VertexBuffer> vb = CreateRef<VertexBuffer>(vertices.data(),
				static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4)));
			vb->SetLayout(layout);

			auto va = CreateRef<VertexArray>();
			va->AddVertexBuffer(vb);

			auto mc = &e.EmplaceComponent<MeshComponent>();
			mc->vertexArray = va;
			mc->renderingMode = RenderingMode::Patches;
			mc->shader = ShaderManager::GetInstance().GetShader(ShaderName::BezierCurve);

			va->Unbind();
		}
		else
		{
			// possible source of bugs but have no way of testing it right now
			auto mc = &e.GetComponent<MeshComponent>();
			mc->vertexArray->GetVertexBuffers()[0]->SetData(vertices.data(),
				static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4)));
			mc->vertexArray->GetVertexBuffers()[0]->SetLayout(layout);
		}
	}
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, BezierCurveC2Component>())
	{
		e.RemoveComponent<IsDirtyTag>();

		const auto& bcc = e.GetComponent<BezierCurveC2Component>();

		std::vector<Algebra::Vector4> vertices;
		std::vector<uint32_t> indices;

		auto& pointHandles = (Entity{ bcc.deBoorPolylineHandle, m_Scene.get() })
			.GetComponent<LineComponent>().pointHandles;

		if (pointHandles.size() < 4)
		{
			continue;
		}

		Entity bernsteinPolyline{ bcc.bernsteinPolylineHandle, m_Scene.get() };

		for (size_t i = 0; i + 3 < pointHandles.size(); ++i)
		{
			auto d0 = Entity { pointHandles[i], m_Scene.get() };
			auto d1 = Entity { pointHandles[i + 1], m_Scene.get() };
			auto d2 = Entity { pointHandles[i + 2], m_Scene.get() };
			auto d3 = Entity { pointHandles[i + 3], m_Scene.get() };

			Algebra::Vector4 D0 = d0.GetComponent<TranslationComponent>().translation;
			Algebra::Vector4 D1 = d1.GetComponent<TranslationComponent>().translation;
			Algebra::Vector4 D2 = d2.GetComponent<TranslationComponent>().translation;
			Algebra::Vector4 D3 = d3.GetComponent<TranslationComponent>().translation;

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
				auto pointEntity = ObjectCreator::CreatePoint(m_Scene);
				pointEntity.EmplaceTag<IsInvisibleTag>();
				pointEntity.GetComponent<TranslationComponent>().SetTranslation(bezierPoints[j]);
				bernsteinPolyline.GetComponent<LineComponent>().pointHandles.push_back(pointEntity.GetHandle());
				pointEntity.EmplaceComponent<VirtualComponent>(bernsteinPolyline.GetHandle());

				vertices.push_back(bezierPoints[j]);
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

			auto va = CreateRef<VertexArray>();
			va->AddVertexBuffer(vb);

			auto mc = &e.EmplaceComponent<MeshComponent>();
			mc->vertexArray = va;
			mc->renderingMode = RenderingMode::Patches;
			mc->shader = ShaderManager::GetInstance().GetShader(ShaderName::BezierCurve);

			va->Unbind();
		}
		else
		{
			// possible source of bugs but have no way of testing it right now
			auto mc = &e.GetComponent<MeshComponent>();
			mc->vertexArray->GetVertexBuffers()[0]->SetData(vertices.data(),
				static_cast<uint32_t>(vertices.size() * sizeof(Algebra::Vector4)));
			mc->vertexArray->GetVertexBuffers()[0]->SetLayout(layout);
		}
	}
}
