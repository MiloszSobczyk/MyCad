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

            if (e.HasComponent<TranslationComponent>())
            {
                auto& tc = e.GetComponent<TranslationComponent>();
                ImGui::DragFloat3(
                    GenerateLabel(e, std::string("Translation")).c_str(),
                    reinterpret_cast<float*>(&tc.translation),
                    0.02f
                );
            }

            ImGui::Separator();
        }

		ImGui::End();
	}
}