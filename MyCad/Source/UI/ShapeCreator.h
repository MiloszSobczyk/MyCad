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

		ImGui::End();
	}
}