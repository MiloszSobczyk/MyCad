#pragma once

#include "ShapeCreatorHelpers.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

#include <iostream>
#include <numbers>

namespace ShapeCreator
{
	inline Entity CreatePoint(Ref<Scene> scene)
	{
		auto point = scene->CreateEntity();

		auto id = point.EmplaceComponent<IdComponent>().id;
		point.EmplaceComponent<NameComponent>().name = "Point_" + std::to_string(id);

		point.EmplaceComponent<PointComponent>();
		point.EmplaceComponent<TranslationComponent>();
		point.EmplaceTag<IsDirtyTag>();

		return point;
	}

	inline Entity CreateTorus(Ref<Scene> scene)
	{
		auto torus = scene->CreateEntity();

		auto id = torus.EmplaceComponent<IdComponent>().id;
		torus.EmplaceComponent<NameComponent>().name = "Torus_" + std::to_string(id);

		torus.EmplaceComponent<TorusComponent>();

		torus.EmplaceComponent<TranslationComponent>();
		torus.EmplaceComponent<RotationComponent>();
		torus.EmplaceComponent<ScalingComponent>();

		torus.EmplaceTag<IsDirtyTag>();

		return torus;
	}

	inline Entity CreatePolyline(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto polyline = scene->CreateEntity();

		auto id = polyline.EmplaceComponent<IdComponent>().id;
		polyline.EmplaceComponent<NameComponent>().name = "Polyline_" + std::to_string(id);

		auto& pc = polyline.EmplaceComponent<PolylineComponent>();
		pc.pointHandles = pointHandles;
		
		polyline.EmplaceTag<IsDirtyTag>();

		for (auto pointHandle : pointHandles)
		{
			Entity pointEntity{ pointHandle, scene.get() };
			auto& nc = pointEntity.EmplaceComponent<NotificationComponent>();
			nc.AddToNotify(polyline.GetHandle());
		}

		return polyline;
	}

	inline Entity CreateBezierCurveC0(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto curveEntity = scene->CreateEntity();

		auto id = curveEntity.EmplaceComponent<IdComponent>().id;
		curveEntity.EmplaceComponent<NameComponent>().name = "BezierCurveC0_" + std::to_string(id);

		curveEntity.EmplaceTag<IsDirtyTag>();

		auto& curveComponent = curveEntity.EmplaceComponent<CurveComponent>();
		
		auto bernsteinPolyline = CreatePolyline(scene, pointHandles);
		bernsteinPolyline.EmplaceComponent<VirtualComponent>(curveEntity.GetHandle());
		bernsteinPolyline.EmplaceTag<IsInvisibleTag>();

		curveComponent.bernsteinPolylineHandle = bernsteinPolyline.GetHandle();

		curveComponent.onUpdate = [scene](CurveComponent& curve) {
			return Curves::C0::UpdateCurve(curve, scene);
			};

		return curveEntity;
	}

	inline Entity CreateBezierCurveC2(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto curveEntity = scene->CreateEntity();

		auto id = curveEntity.EmplaceComponent<IdComponent>().id;
		curveEntity.EmplaceComponent<NameComponent>().name = "BezierCurveC2_" + std::to_string(id);

		curveEntity.EmplaceTag<IsDirtyTag>();

		auto& curveComponent = curveEntity.EmplaceComponent<CurveComponent>();

		auto deBoorPolyline = CreatePolyline(scene, pointHandles);
		deBoorPolyline.EmplaceComponent<VirtualComponent>(curveEntity.GetHandle());
		deBoorPolyline.EmplaceTag<IsInvisibleTag>();

		curveComponent.controlPolylineHandle = deBoorPolyline.GetHandle();

		auto bernsteinPolyline = CreatePolyline(scene, {});
		bernsteinPolyline.EmplaceComponent<VirtualComponent>(curveEntity.GetHandle());
		bernsteinPolyline.EmplaceTag<IsInvisibleTag>();

		curveComponent.bernsteinPolylineHandle = bernsteinPolyline.GetHandle();

		curveComponent.onUpdate = [scene](CurveComponent& curve) {
			return Curves::C2::UpdateCurve(curve, scene);
			};

		return curveEntity;
	}

	inline Entity CreateInterpolatingCurve(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto curveEntity = scene->CreateEntity();

		auto id = curveEntity.EmplaceComponent<IdComponent>().id;
		curveEntity.EmplaceComponent<NameComponent>().name = "InterpolatingCurve_" + std::to_string(id);

		curveEntity.EmplaceTag<IsDirtyTag>();

		auto& curveComponent = curveEntity.EmplaceComponent<CurveComponent>();

		auto interpolatingPolyline = CreatePolyline(scene, pointHandles);
		interpolatingPolyline.EmplaceComponent<VirtualComponent>(curveEntity.GetHandle());
		interpolatingPolyline.EmplaceTag<IsInvisibleTag>();

		curveComponent.controlPolylineHandle = interpolatingPolyline.GetHandle();

		auto bernsteinPolyline = CreatePolyline(scene, {});
		bernsteinPolyline.EmplaceComponent<VirtualComponent>(curveEntity.GetHandle());
		bernsteinPolyline.EmplaceTag<IsInvisibleTag>();

		curveComponent.bernsteinPolylineHandle = bernsteinPolyline.GetHandle();

		curveComponent.onUpdate = [scene](CurveComponent& curve) {
			return Curves::Interpolating::UpdateCurve(curve, scene);
			};

		return curveEntity;
	}

	inline Entity CreateBezierSurfaceC0(Ref<Scene> scene, Algebra::Vector4 position = { 0.f, 0.f, 0.f, 0.f })
	{
		auto surface = scene->CreateEntity();

		// ID & Name
		auto id = surface.EmplaceComponent<IdComponent>().id;
		surface.EmplaceComponent<NameComponent>().name = "BezierSurfaceC0_" + std::to_string(id);
		surface.EmplaceTag<IsDirtyTag>();

		auto& bsc = surface.EmplaceComponent<BezierSurfaceComponent>();
		auto controlPoints = Surfaces::SetupControlPoints(bsc, position, 2.f, 2.f);

		// Create points
		bsc.pointHandles.reserve(controlPoints.size());
		for (size_t i = 0; i < controlPoints.size(); ++i) 
		{
			Entity point = ShapeCreator::CreatePoint(scene);
			point.GetComponent<TranslationComponent>().SetTranslation(controlPoints[i]);

			bsc.pointHandles.push_back(point.GetHandle());
			auto& nc = point.EmplaceComponent<NotificationComponent>();
			nc.AddToNotify(surface.GetHandle());
		}

		// Create patches
		bsc.patchHandles.reserve(bsc.widthPatches * bsc.heightPatches);
		for (int patchIndex = 0; patchIndex < bsc.widthPatches * bsc.heightPatches; ++patchIndex) 
		{
			Entity patchEntity = scene->CreateEntity();
			patchEntity.EmplaceTag<IsDirtyTag>();
			patchEntity.EmplaceComponent<VirtualComponent>(surface.GetHandle());
			
			auto& patchComponent = patchEntity.EmplaceComponent<PatchComponent>();
			patchComponent.bernsteinPolylineHandle = CreatePolyline(scene, {}).GetHandle();

			int startI = patchIndex / bsc.widthPatches;
			int startJ = patchIndex % bsc.widthPatches;

			std::vector<entt::entity> handles;
			handles.reserve(16);
			for (int i = 0; i < 4; ++i) 
			{
				for (int j = 0; j < 4; ++j) 
				{
					auto pointHandle = bsc.pointHandles[(startI * 3 + i) * bsc.GetColumns() + startJ * 3 + j];
					auto pointEntity = Entity{ pointHandle, scene.get() };
					pointEntity.GetComponent<NotificationComponent>().AddToNotify(patchEntity.GetHandle());

					handles.push_back(pointHandle);
				}
			}

			patchComponent.pointHandles = handles;
			patchComponent.onUpdate = [scene](PatchComponent& patch) {
				return Surfaces::C0::UpdatePatch(patch, scene);
				};
			Surfaces::InitializePatchPolylinePoints(Entity{ patchComponent.bernsteinPolylineHandle, scene.get() }, handles, scene);

			bsc.patchHandles.push_back(patchEntity.GetHandle());
		}

		return surface;
	}

	inline Entity CreateBezierSurfaceC2(Ref<Scene> scene, Algebra::Vector4 position = { 0.f, 0.f, 0.f, 0.f })
	{
		auto surface = scene->CreateEntity();

		// ID & Name
		auto id = surface.EmplaceComponent<IdComponent>().id;
		surface.EmplaceComponent<NameComponent>().name = "BezierSurfaceC2_" + std::to_string(id);
		surface.EmplaceTag<IsDirtyTag>();

		auto& bsc = surface.EmplaceComponent<BezierSurfaceComponent>();
		bsc.C2 = true;
		auto controlPoints = Surfaces::SetupControlPoints(bsc, position, 2.f, 2.f);

		// Create points
		bsc.pointHandles.reserve(controlPoints.size());
		for (size_t i = 0; i < controlPoints.size(); ++i)
		{
			Entity point = ShapeCreator::CreatePoint(scene);
			point.GetComponent<TranslationComponent>().SetTranslation(controlPoints[i]);
			point.EmplaceTag<IsInvisibleTag>();

			bsc.pointHandles.push_back(point.GetHandle());
			auto& nc = point.EmplaceComponent<NotificationComponent>();
			nc.AddToNotify(surface.GetHandle());
		}

		// Create patches
		bsc.patchHandles.reserve(bsc.widthPatches * bsc.heightPatches);
		for (int patchIndex = 0; patchIndex < bsc.widthPatches * bsc.heightPatches; ++patchIndex)
		{
			Entity patchEntity = scene->CreateEntity();
			patchEntity.EmplaceTag<IsDirtyTag>();
			patchEntity.EmplaceComponent<VirtualComponent>(surface.GetHandle());

			auto& patchComponent = patchEntity.EmplaceComponent<PatchComponent>();
			patchComponent.deBoorPolylineHandle = CreatePolyline(scene, {}).GetHandle();
			patchComponent.bernsteinPolylineHandle = CreatePolyline(scene, {}).GetHandle();

			int startI = patchIndex / bsc.widthPatches;
			int startJ = patchIndex % bsc.widthPatches;

			std::vector<entt::entity> deBoorPointHandles;
			deBoorPointHandles.reserve(16);
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					auto pointHandle = bsc.pointHandles[(startI + i) * bsc.GetColumns() + startJ + j];
					auto pointEntity = Entity{ pointHandle, scene.get() };
					pointEntity.GetComponent<NotificationComponent>().AddToNotify(patchEntity.GetHandle());

					deBoorPointHandles.push_back(pointHandle);
				}
			}

			patchComponent.pointHandles = deBoorPointHandles;

			auto& bernsteinPolyline = Entity{ patchComponent.bernsteinPolylineHandle, scene.get() }.GetComponent<PolylineComponent>();
			patchComponent.onUpdate = [scene, &bernsteinPolyline](PatchComponent& patch) {
				return Surfaces::C2::UpdatePatch(patch, scene, bernsteinPolyline);
				};
			Surfaces::InitializePatchPolylinePoints(Entity{ patchComponent.deBoorPolylineHandle, scene.get() }, deBoorPointHandles, scene);

			auto bernsteinPointHandles = std::vector<entt::entity>{};
			bernsteinPointHandles.reserve(16);
			for (int i = 0; i < 16; ++i)
			{
				Entity point = ShapeCreator::CreatePoint(scene);
				point.EmplaceTag<IsInvisibleTag>();

				bernsteinPointHandles.push_back(point.GetHandle());
			}
			Surfaces::InitializePatchPolylinePoints(Entity{ patchComponent.bernsteinPolylineHandle, scene.get() }, bernsteinPointHandles, scene, false);

			bsc.patchHandles.push_back(patchEntity.GetHandle());
		}

		return surface;
	}
}