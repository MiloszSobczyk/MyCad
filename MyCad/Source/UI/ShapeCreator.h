#pragma once

#include "Core/Scene/Scene.h"
#include "Creators/ObjectCreator.h"

#include <imgui/imgui.h>

namespace UI
{
	inline std::vector<entt::entity> GetSelectedPointHandles(Ref<Scene> scene)
	{
		std::vector<entt::entity> pointHandles;
		for (auto e : scene->GetAllEntitiesWith<IsSelectedTag, TranslationComponent>())
		{
			auto& tc = e.GetComponent<TranslationComponent>();
			pointHandles.push_back(tc.translation.entity.GetHandle());
		}
		return pointHandles;
	}

	inline void DisplayShapeCreator(Ref<Scene> scene)
	{
		ImGui::Begin("Shape creation##shape_creation");

		if (ImGui::Button("Create Torus##shape_creation"))
		{
			ObjectCreator::CreateTorus(scene);
		}
		if (ImGui::Button("Create Point##shape_creation"))
		{
			ObjectCreator::CreatePoint(scene);
		}
		if (ImGui::Button("Create Polyline##shape_creation"))
		{
			auto pointHandles = GetSelectedPointHandles(scene);
			if (!pointHandles.empty())
			{
				auto polyline = ObjectCreator::CreatePolyline(scene, pointHandles);

				for (auto pointHandle : pointHandles)
				{
					Entity pointEntity{ pointHandle, scene.get() };
					auto& nc = pointEntity.EmplaceComponent<NotificationComponent>();
					nc.AddToNotify(polyline.GetHandle());
				}
			}
		}
		if (ImGui::Button("Create Bezier Curve C0##shape_creation"))
		{
			auto pointHandles = GetSelectedPointHandles(scene);
			if (!pointHandles.empty())
			{
				auto polyline = ObjectCreator::CreateBezierCurveC0(scene, pointHandles);

				for (auto pointHandle : pointHandles)
				{
					Entity pointEntity{ pointHandle, scene.get() };
					auto& nc = pointEntity.EmplaceComponent<NotificationComponent>();
					nc.AddToNotify(polyline.GetHandle());
				}
			}
		}

		ImGui::End();
	}
}