#include "MeshGenerationSystem.h"

#include "Core/Scene/Entity.h"
#include "Creators/ShapeCreator.h"
#include "ECS/Components/Components.h"
#include "Creators/MeshCreator.h"

#include <numbers>

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

		MeshCreator::MeshData mesh = MeshCreator::GenerateTorusMeshData(tc);

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout, RenderingMode::Lines, ShaderName::Default);
	}
}

void MeshGenerationSystem::UpdatePointMeshes()
{
	MeshCreator::MeshData mesh = MeshCreator::GeneratePointMeshData();

	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, PointComponent>())
	{
		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout, RenderingMode::Triangles, ShaderName::Default);
	}
}

// Removing point from mesh should also remove mesh's reference from point
void MeshGenerationSystem::UpdateLineMeshes()
{
	BufferLayout layout = {
		{ ShaderDataType::Float4, "position" }
	};

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

		MeshCreator::UpdateMesh(e, vertices, indices, layout, RenderingMode::Lines, ShaderName::Default);
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

		MeshCreator::UpdateMesh(e, vertices, indices, layout, RenderingMode::Patches, ShaderName::BezierCurve);
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
				auto pointEntity = ShapeCreator::CreatePoint(m_Scene);
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

		MeshCreator::UpdateMesh(e, vertices, indices, layout, RenderingMode::Patches, ShaderName::BezierCurve);
	}
}
