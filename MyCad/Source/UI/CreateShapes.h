#pragma once

#include "Core/Scene/Scene.h"
#include "Creators/ShapeCreator.h"

#include "Algebra.h"

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
		static bool openSurfacePopup = false;
		static Algebra::Vector4 surfacePosition;

		ImGui::Begin("Shape creation##shape_creation");

		if (ImGui::Button("Create Point##shape_creation"))
		{
			ShapeCreator::CreatePoint(scene);
		}
		if (ImGui::Button("Create Torus##shape_creation"))
		{
			ShapeCreator::CreateTorus(scene);
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
		if (ImGui::Button("Create Gregory Patch##shape_creation"))
		{
			ShapeCreator::CreateGregoryPatch(scene);
		}

		if (ImGui::Button("Create Bezier Surface##shape_creation"))
		{
			openSurfacePopup = true;
		}

		ImGui::End();

		if (openSurfacePopup)
		{
			ImGui::OpenPopup("Choose Surface Position##surface_position");
			openSurfacePopup = false;
		}

		if (ImGui::BeginPopupModal("Choose Surface Position##surface_position", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::SliderFloat("X", &surfacePosition.x, -5.f, 5.f);
			ImGui::SliderFloat("Y", &surfacePosition.y, -5.f, 5.f);
			ImGui::SliderFloat("Z", &surfacePosition.z, -5.f, 5.f);
			ImGui::SliderFloat("W", &surfacePosition.w, -5.f, 5.f);

			if (ImGui::Button("Create C0 Surface"))
			{
				ShapeCreator::CreateBezierSurfaceC0(scene, { surfacePosition });
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Create C2 Surface"))
			{
				ShapeCreator::CreateBezierSurfaceC2(scene, { surfacePosition });
				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}
}
