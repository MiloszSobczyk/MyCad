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
		point.EmplaceComponent<NameComponent>().name = "Point" + std::to_string(id);

		point.EmplaceComponent<PointComponent>();
		point.EmplaceComponent<TranslationComponent>();
		point.EmplaceTag<IsDirtyTag>();

		return point;
	}

	inline Entity CreateTorus(Ref<Scene> scene)
	{
		auto torus = scene->CreateEntity();

		auto id = torus.EmplaceComponent<IdComponent>().id;
		torus.EmplaceComponent<NameComponent>().name = "Torus" + std::to_string(id);

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
		polyline.EmplaceComponent<NameComponent>().name = "Polyline" + std::to_string(id);

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
		curve.EmplaceComponent<NameComponent>().name = "BezierCurveC0" + std::to_string(id);

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
		curve.EmplaceComponent<NameComponent>().name = "BezierCurveC2" + std::to_string(id);

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
}