#pragma once

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

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

		auto& pc = polyline.EmplaceComponent<LineComponent>();
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
		auto curve = scene->CreateEntity();

		auto id = curve.EmplaceComponent<IdComponent>().id;
		curve.EmplaceComponent<NameComponent>().name = "BezierCurveC0_" + std::to_string(id);

		curve.EmplaceTag<IsDirtyTag>();

		auto& bcc = curve.EmplaceComponent<BezierCurveC0Component>();
		
		auto polyline = CreatePolyline(scene, pointHandles);
		polyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		polyline.EmplaceTag<IsInvisibleTag>();

		bcc.polylineHandle = polyline.GetHandle();

		return curve;
	}

	inline Entity CreateBezierCurveC2(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto curve = scene->CreateEntity();

		auto id = curve.EmplaceComponent<IdComponent>().id;
		curve.EmplaceComponent<NameComponent>().name = "BezierCurveC2_" + std::to_string(id);

		curve.EmplaceTag<IsDirtyTag>();

		auto& bcc = curve.EmplaceComponent<BezierCurveC2Component>();

		auto deBoorPolyline = CreatePolyline(scene, pointHandles);
		deBoorPolyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		deBoorPolyline.EmplaceTag<IsInvisibleTag>();

		bcc.deBoorPolylineHandle = deBoorPolyline.GetHandle();

		auto bernsteinPolyline = CreatePolyline(scene, {});
		bernsteinPolyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		bernsteinPolyline.EmplaceTag<IsInvisibleTag>();

		bcc.bernsteinPolylineHandle = bernsteinPolyline.GetHandle();

		return curve;
	}

	inline Entity CreateInterpolatingCurve(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto curve = scene->CreateEntity();

		auto id = curve.EmplaceComponent<IdComponent>().id;
		curve.EmplaceComponent<NameComponent>().name = "InterpolatingCurve_" + std::to_string(id);

		curve.EmplaceTag<IsDirtyTag>();

		auto& icc = curve.EmplaceComponent<InterpolatingCurveComponent>();

		auto interpolatingPolyline = CreatePolyline(scene, pointHandles);
		interpolatingPolyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		interpolatingPolyline.EmplaceTag<IsInvisibleTag>();

		icc.interpolatingPolylineHandle = interpolatingPolyline.GetHandle();

		auto bernsteinPolyline = CreatePolyline(scene, {});
		bernsteinPolyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		bernsteinPolyline.EmplaceTag<IsInvisibleTag>();

		icc.bernsteinPolylineHandle = bernsteinPolyline.GetHandle();

		return curve;
	}

	inline Entity CreateBezierSurfaceC0(Ref<Scene> scene)
	{
		auto surface = scene->CreateEntity();

		auto id = surface.EmplaceComponent<IdComponent>().id;
		surface.EmplaceComponent<NameComponent>().name = "BezierSurfaceC0_" + std::to_string(id);

		surface.EmplaceTag<IsDirtyTag>();

		auto& bsc = surface.EmplaceComponent<BezierSurfaceC0Component>();
		for (int i = 0; i < bsc.GetColumns() * bsc.GetRows(); ++i)
		{
			Entity pointEntity = ShapeCreator::CreatePoint(scene);
			pointEntity.EmplaceTag<IsInvisibleTag>();
			bsc.pointHandles.push_back(pointEntity.GetHandle());
			// Probably here
			//auto& nc = pointEntity.EmplaceComponent<NotificationComponent>();
			//nc.AddToNotify(surface.GetHandle());
		}

		for (int patchIndex = 0; patchIndex < bsc.widthPatches * bsc.heightPatches; ++patchIndex)
		{
			Entity patchEntity = scene->CreateEntity();
			auto& pc = patchEntity.EmplaceComponent<PatchComponent>();

			int startingI = patchIndex / bsc.widthPatches;
			int startingJ = patchIndex % bsc.widthPatches;

			std::vector<entt::entity> controlPoints;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					controlPoints.push_back(bsc.pointHandles[(startingI * 3 + i) * bsc.GetColumns() + startingJ * 3 + j]);
				}
			}

			pc.pointHandles = controlPoints;
			pc.bernsteinPolylineHandle = ShapeCreator::CreatePolyline(scene, controlPoints).GetHandle();
			bsc.patchHandles.push_back(patchEntity.GetHandle());
		}

		return surface;
	}
}