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
}