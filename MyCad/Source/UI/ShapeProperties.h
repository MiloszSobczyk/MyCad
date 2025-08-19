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

        if (ImGui::Button("Refresh polyline"))
        {
            for(auto entity : scene->GetAllEntitiesWith<PolylineComponent>())
            {
                Entity e{ entity, scene.get() };
                e.EmplaceTag<IsDirtyTag>();
			}
        }

        for (auto entity : selectedShapes)
        {
            Entity e{ entity, scene.get() };

            ImGui::Text("%s", GenerateLabel(e, e.GetComponent<NameComponent>().name).c_str());

            if (e.HasComponent<TranslationComponent>())
            {
                auto& tc = e.GetComponent<TranslationComponent>();

                Algebra::Vector4 translation = tc.translation;
                float temp[3] = { translation.x, translation.y, translation.z };

                if (ImGui::DragFloat3(
                    GenerateLabel(e, std::string("Translation")).c_str(),
                    temp,
                    0.02f
                ))
                {
                    tc.SetTranslation({ temp[0], temp[1], temp[2] });
                }
            }


            ImGui::Separator();
        }

		ImGui::End();
	}
}