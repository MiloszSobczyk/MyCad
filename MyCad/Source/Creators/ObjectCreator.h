#pragma once

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

namespace ObjectCreator
{
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

	inline Entity CreatePolyline(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto polyline = scene->CreateEntity();

		auto id = polyline.EmplaceComponent<IdComponent>().id;
		polyline.EmplaceComponent<NameComponent>().name = "Polyline" + std::to_string(id);

		auto& pc = polyline.EmplaceComponent<LineComponent>();
		pc.pointHandles = pointHandles;
		
		polyline.EmplaceTag<IsDirtyTag>();

		return polyline;
	}

	inline Entity CreateBezierCurveC0(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto polyline = scene->CreateEntity();

		auto id = polyline.EmplaceComponent<IdComponent>().id;
		polyline.EmplaceComponent<NameComponent>().name = "BezierCyurveC0" + std::to_string(id);

		auto& pc = polyline.EmplaceComponent<LineComponent>();
		pc.pointHandles = pointHandles;

		polyline.EmplaceTag<IsDirtyTag>();
		polyline.EmplaceTag<IsBezierCurveC0Tag>();

		return polyline;
	}
}