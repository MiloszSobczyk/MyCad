#pragma once

#include "Core/Scene/Scene.h"
#include "Creators/ShapeCreator.h"

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
			ShapeCreator::CreateTorus(scene);
		}
		if (ImGui::Button("Create Point##shape_creation"))
		{
			ShapeCreator::CreatePoint(scene);
		}
		if (ImGui::Button("Create Polyline##shape_creation"))
		{
			auto pointHandles = GetSelectedPointHandles(scene);
			if (!pointHandles.empty())
			{
				ShapeCreator::CreatePolyline(scene, pointHandles);
			}
		}
		if (ImGui::Button("Create Bezier Curve C0##shape_creation"))
		{
			auto pointHandles = GetSelectedPointHandles(scene);
			if (!pointHandles.empty())
			{
				ShapeCreator::CreateBezierCurveC0(scene, pointHandles);
			}
		}
		if (ImGui::Button("Create Bezier Curve C2##shape_creation"))
		{
			auto pointHandles = GetSelectedPointHandles(scene);
			if (!pointHandles.empty())
			{
				ShapeCreator::CreateBezierCurveC2(scene, pointHandles);
			}
		}
		if (ImGui::Button("Create Interpolating Curve##shape_creation"))
		{
			auto pointHandles = GetSelectedPointHandles(scene);
			if (!pointHandles.empty())
			{
				ShapeCreator::CreateInterpolatingCurve(scene, pointHandles);
			}
		}

		ImGui::End();
	}
}