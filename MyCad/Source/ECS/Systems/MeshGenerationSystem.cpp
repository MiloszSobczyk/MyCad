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
	UpdatePatchMeshes();
	UpdateGregoryPatchMeshes();
}

void MeshGenerationSystem::UpdateDirtyTags()
{
	for (auto e : m_Scene->GetAllEntitiesWith<VirtualComponent, DirtyFromComponent>())
	{
		auto& vc = e.GetComponent<VirtualComponent>();
		Entity targetEntity{ vc.targetEntity, m_Scene.get() };
		targetEntity.EmplaceTag<DirtyFromComponent>();
	}
}

void MeshGenerationSystem::UpdatePointMeshes()
{
	MeshCreator::MeshData mesh = MeshCreator::Point::GenerateMeshData();

	for (auto e : m_Scene->GetAllEntitiesWith<DirtyFromComponent, PointComponent>())
	{
		e.RemoveComponent<DirtyFromComponent>();

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout, RenderingMode::Triangles);
	}
}

void MeshGenerationSystem::UpdateTorusMeshes()
{
	for (auto e : m_Scene->GetAllEntitiesWith<DirtyFromComponent, TorusComponent>())
	{
		e.RemoveComponent<DirtyFromComponent>();

		const auto& tc = e.GetComponent<TorusComponent>();

		MeshCreator::MeshData mesh = MeshCreator::Torus::GenerateMeshData(tc);

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout);
	}
}

// Removing point from mesh should also remove mesh's reference from point, so I will need ShapeManagementSystem
void MeshGenerationSystem::UpdateLineMeshes()
{
	for (auto e : m_Scene->GetAllEntitiesWith<DirtyFromComponent, PolylineComponent>())
	{
		e.RemoveComponent<DirtyFromComponent>();

		const auto& pc = e.GetComponent<PolylineComponent>();

		if (pc.pointHandles.empty())
		{
			continue;
		}
		
		MeshCreator::MeshData mesh = MeshCreator::Polyline::GenerateMeshData(pc, m_Scene);
		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout);
	}
	for (auto e : m_Scene->GetAllEntitiesWith<DirtyFromComponent, CurveComponent>())
	{
		e.RemoveComponent<DirtyFromComponent>();

		auto& curveComponent = e.GetComponent<CurveComponent>();

		MeshCreator::MeshData mesh = MeshCreator::Curve::GenerateMeshData(curveComponent, m_Scene);

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout, 
			RenderingMode::Patches, { ShaderName::BezierCurve });
	}
}

void MeshGenerationSystem::UpdatePatchMeshes()
{
	for (auto e : m_Scene->GetAllEntitiesWith<DirtyFromComponent, PatchComponent>())
	{
		e.RemoveComponent<DirtyFromComponent>();

		auto& pc = e.GetComponent<PatchComponent>();
		
		MeshCreator::MeshData mesh = MeshCreator::Patch::GenerateMeshData(pc, m_Scene);

		MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout,
			RenderingMode::Patches, { ShaderName::BezierSurface, ShaderName::BezierSurface2 });
	}
}

void MeshGenerationSystem::UpdateGregoryPatchMeshes()
{
	for (auto e : m_Scene->GetAllEntitiesWith<DirtyFromComponent, GregoryPatchComponent>())
	{
		e.RemoveComponent<DirtyFromComponent>();
		auto& gpc = e.GetComponent<GregoryPatchComponent>();

		MeshCreator::MeshData mesh = MeshCreator::GregoryPatch::GenerateMeshData(gpc, m_Scene);

		//MeshCreator::UpdateMesh(e, mesh.vertices, mesh.indices, mesh.layout,
			//RenderingMode::Patches, { ShaderName::GregoryPatch });
	}
}

