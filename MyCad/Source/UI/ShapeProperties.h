#pragma once

#include "Core/Scene/Scene.h"
#include "ECS/Components/Components.h"
#include "Utils.h"
#include "Utils/Base.h"


namespace UI
{
	inline void DisplaySelectedShapes(Ref<Scene> scene)
	{
		ImGui::Begin("Selected Shapes##selected_shapes");

		auto selectedShapes = scene->GetAllEntitiesWith<IsSelectedTag>();
		if (selectedShapes.empty())
		{
			ImGui::Text("No shapes selected##selected_shapes");
			ImGui::End();
			return;
		}

		for (auto entity : selectedShapes)
		{
			Entity e{ entity, scene.get() };
			ImGui::Text("%s", GenerateLabel(e, e.GetComponent<NameComponent>().name).c_str());
		}

		ImGui::End();
	}
}