#include "MeshGenerationSystem.h"

#include "Core/Scene/Entity.h"
#include "Creators/ShapeCreator.h"
#include "ECS/Components/Components.h"
#include "Creators/MeshCreator.h"

#include <numbers>
#include <array>

MeshGenerationSystem::MeshGenerationSystem(Ref<Scene> scene)
	: m_Scene{ scene }
{
}

void MeshGenerationSystem::Update()
{
	UpdateDirtyTags();
	UpdatePointMeshes();
	UpdateTorusMeshes();
	UpdateLineMeshes();
	UpdateSurfaceMeshes();
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

void MeshGenerationSystem::UpdatePointMeshes()
{
	MeshCreator::MeshData mesh = MeshCreator::GeneratePointMeshData();

	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, PointComponent>())
	{
		e.RemoveComponent<IsDirtyTag>();

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout, RenderingMode::Triangles);
	}
}

void MeshGenerationSystem::UpdateTorusMeshes()
{
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, TorusComponent>())
	{
		e.RemoveComponent<IsDirtyTag>();

		const auto& tc = e.GetComponent<TorusComponent>();

		MeshCreator::MeshData mesh = MeshCreator::GenerateTorusMeshData(tc);

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout);
	}
}

// Removing point from mesh should also remove mesh's reference from point, so I will need ShapeManagementSystem
void MeshGenerationSystem::UpdateLineMeshes()
{
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, LineComponent>())
	{
		e.RemoveComponent<IsDirtyTag>();

		const auto& pc = e.GetComponent<LineComponent>();

		if (pc.pointHandles.empty())
		{
			continue;
		}
		
		MeshCreator::MeshData mesh = MeshCreator::GenerateLineMeshData(pc, m_Scene);
		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout);
	}
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, BezierCurveC0Component>())
	{
		e.RemoveComponent<IsDirtyTag>();

		const auto& bcc = e.GetComponent<BezierCurveC0Component>();

		MeshCreator::MeshData mesh = MeshCreator::GenerateBezierC0MeshData(bcc, m_Scene);

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout, 
			RenderingMode::Patches, { ShaderName::BezierCurve });
	}
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, BezierCurveC2Component>())
	{
		e.RemoveComponent<IsDirtyTag>();

		const auto& bcc = e.GetComponent<BezierCurveC2Component>();

		MeshCreator::MeshData mesh = MeshCreator::GenerateBezierC2MeshData(bcc, m_Scene);

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout, 
			RenderingMode::Patches, { ShaderName::BezierCurve });
	}
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, InterpolatingCurveComponent>())
	{
		e.RemoveComponent<IsDirtyTag>();

		const auto& icc = e.GetComponent<InterpolatingCurveComponent>();

		MeshCreator::MeshData mesh = MeshCreator::GenerateInterpolatingCurveMeshData(icc, m_Scene);

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout,
			RenderingMode::Patches, { ShaderName::BezierCurve });
	}
}

void MeshGenerationSystem::UpdateSurfaceMeshes()
{
	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, BezierSurfaceComponent>())
	{
		e.RemoveComponent<IsDirtyTag>();

		auto& bsc = e.GetComponent<BezierSurfaceComponent>();
		MeshCreator::MeshData mesh = MeshCreator::GenerateBezierSurfaceC0MeshData(bsc, m_Scene);

		for (int patchIndex = 0; patchIndex < bsc.widthPatches * bsc.heightPatches; ++patchIndex)
		{
			Entity patchEntity{ bsc.patchHandles[patchIndex], m_Scene.get() };
			auto& pc = patchEntity.GetComponent<PatchComponent>();

			int startIndex = patchIndex * 16;
			std::vector<entt::entity> controlPoints;
			for (int i = 0; i < 16; ++i)
			{
				Entity pointEntity{ pc.pointHandles[i], m_Scene.get() };
				pointEntity.GetComponent<TranslationComponent>().SetTranslation(mesh.vertices[startIndex + i]);
			}
		}

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout,
			RenderingMode::Patches, { ShaderName::BezierSurface, ShaderName::BezierSurface2 });
	}

	for (auto e : m_Scene->GetAllEntitiesWith<IsDirtyTag, BezierSurfaceC2Component>())
	{
		e.RemoveComponent<IsDirtyTag>();

		auto& bsc = e.GetComponent<BezierSurfaceC2Component>();
		MeshCreator::MeshData mesh = MeshCreator::GenerateBezierSurfaceC2MeshData(bsc, m_Scene);

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout,
			RenderingMode::Patches, { ShaderName::BezierSurface, ShaderName::BezierSurface2 });
	}
}
