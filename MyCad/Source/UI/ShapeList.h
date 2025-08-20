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
			ImGui::Text("No shapes available##shape_list");
			ImGui::End();
			return;
		}

		for (auto e : shapes)
		{
			bool isSelected = e.HasComponent<IsSelectedTag>();

			if (ImGui::Selectable(GenerateLabel(e, e.GetComponent<NameComponent>().name).c_str(), isSelected))
			{
				if (isSelected)
				{
					e.RemoveComponent<IsSelectedTag>();
				}
				else
				{
					e.EmplaceTag<IsSelectedTag>();
				}
			}
		}

		ImGui::End();
	}
}