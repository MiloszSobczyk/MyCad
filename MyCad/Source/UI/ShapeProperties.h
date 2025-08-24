#pragma once

#include "Core/Scene/Scene.h"
#include "ECS/Components/Components.h"
#include "Utils.h"
#include "Utils/Base.h"


namespace UI
{
    inline void DisplayTranslationComponentUI(Entity& entity, TranslationComponent& tc)
    {
        Algebra::Vector4 translation = tc.translation;
        float temp[3] = { translation.x, translation.y, translation.z };
        if (ImGui::DragFloat3(
            GenerateLabel(entity, std::string("Translation")).c_str(),
            temp,
            0.02f
        ))
        {
            tc.SetTranslation({ temp[0], temp[1], temp[2] });
        }
	}

    inline bool DisplayPolylineComponentUI(Entity& entity, LineComponent& lc, Ref<Scene> scene)
    {
        bool changed = false;
        ImGui::Text("Control Points:");

        for (size_t i = 0; i < lc.pointHandles.size(); ++i)
        {
            Entity pointEntity{ lc.pointHandles[i], scene.get() };

            ImGui::PushID(static_cast<int>(i));

            if (ImGui::Button("X"))
            {
                lc.pointHandles.erase(lc.pointHandles.begin() + i);
                entity.EmplaceTag<IsDirtyTag>();
                ImGui::PopID();
                changed = true;
                break;
            }

            ImGui::SameLine();

            if (i == 0) ImGui::BeginDisabled();
            if (ImGui::ArrowButton("##up", ImGuiDir_Up))
            {
                std::swap(lc.pointHandles[i], lc.pointHandles[i - 1]);
                entity.EmplaceTag<IsDirtyTag>();
                changed = true;
            }
            if (i == 0) ImGui::EndDisabled();

            ImGui::SameLine();

            if (i + 1 == lc.pointHandles.size()) ImGui::BeginDisabled();
            if (ImGui::ArrowButton("##down", ImGuiDir_Down))
            {
                std::swap(lc.pointHandles[i], lc.pointHandles[i + 1]);
                entity.EmplaceTag<IsDirtyTag>();
                changed = true;
            }
            if (i + 1 == lc.pointHandles.size()) ImGui::EndDisabled();

            ImGui::SameLine();

            ImGui::Text("%s", pointEntity.GetComponent<NameComponent>().name.c_str());

            ImGui::PopID();
        }

        return changed;
    }

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
            for(auto e : scene->GetAllEntitiesWith<LineComponent>())
            {
                e.EmplaceTag<IsDirtyTag>();
			}
        }

        for (auto e : selectedShapes)
        {
            ImGui::Text("%s", GenerateLabel(e, e.GetComponent<NameComponent>().name).c_str());

            if (e.HasComponent<TranslationComponent>())
            {
                auto& tc = e.GetComponent<TranslationComponent>();
				DisplayTranslationComponentUI(e, tc);
            }
            if (e.HasComponent<LineComponent>())
            {
				auto& lc = e.GetComponent<LineComponent>();
				DisplayPolylineComponentUI(e, lc, scene);
            }

            ImGui::Separator();
        }

		ImGui::End();
	}
}