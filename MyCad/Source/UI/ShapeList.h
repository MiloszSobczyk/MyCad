#pragma once

#include "Core/Scene/Scene.h"
#include "ECS/Components/Components.h"
#include "Utils.h"
#include "Utils/Base.h"

namespace UI
{
	void DisplayShapeList(Ref<Scene> scene)
	{
		ImGui::Begin("Shape List##shape_list");

		auto shapes = scene->GetAllEntitiesWith<NameComponent>();

		if (shapes.empty())
		{
			ImGui::Text("No shapes available.##shape_list");
			ImGui::End();
			return;
		}

		for (auto entity : shapes)
		{
			Entity e{ entity, scene.get() };

			if (ImGui::Selectable(GenerateLabel(e, e.GetComponent<NameComponent>().name).c_str(), false))
			{
			}
		}

		ImGui::End();
	}
}