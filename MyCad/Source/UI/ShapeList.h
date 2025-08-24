#pragma once

#include "Core/Scene/Scene.h"
#include "ECS/Components/Components.h"
#include "Utils.h"
#include "Utils/Base.h"

namespace UI
{
	void DisplayDeselectShapesButton(Ref<Scene> scene)
	{
		if (ImGui::Button("Deselect all"))
		{
			auto selectedShapes = scene->GetAllEntitiesWith<IsSelectedTag>();
			for (auto e : selectedShapes)
			{
				e.RemoveComponent<IsSelectedTag>();
			}
		}
	}

	void DisplayShapeList(Ref<Scene> scene)
	{
		ImGui::Begin("Shape List##shape_list");

		auto shapes = scene->GetAllEntitiesWith<NameComponent>(entt::exclude<VirtualComponent>);

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

		DisplayDeselectShapesButton(scene);

		ImGui::End();
	}
}