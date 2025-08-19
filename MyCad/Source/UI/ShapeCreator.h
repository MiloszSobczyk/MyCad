#pragma once

#include "Core/Scene/Scene.h"
#include "Creators/ObjectCreator.h"

#include <imgui/imgui.h>

namespace UI
{
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
			std::vector<entt::entity> pointHandles;
			for (auto entity : scene->GetAllEntitiesWith<IsSelectedTag, TranslationComponent>())
			{
				Entity e{ entity, scene.get() };
				auto& tc = e.GetComponent<TranslationComponent>();
				pointHandles.push_back(tc.translation.entity.GetHandle());
			}
			if (!pointHandles.empty())
			{
				ObjectCreator::CreatePolyline(scene, pointHandles);
			}
			else
			{
				ImGui::Text("No points selected to create a polyline.");
			}
		}

		ImGui::End();
	}
}