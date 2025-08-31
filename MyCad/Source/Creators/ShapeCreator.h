#pragma once

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

#include <iostream>
#include <numbers>

namespace ShapeCreator
{
	// Split these into multiple namespaces

	inline Entity CreatePoint(Ref<Scene> scene)
	{
		auto point = scene->CreateEntity();

		auto id = point.EmplaceComponent<IdComponent>().id;
		point.EmplaceComponent<NameComponent>().name = "Point_" + std::to_string(id);

		point.EmplaceComponent<PointComponent>();
		point.EmplaceComponent<TranslationComponent>();
		point.EmplaceTag<IsDirtyTag>();

		return point;
	}

	inline Entity CreateTorus(Ref<Scene> scene)
	{
		auto torus = scene->CreateEntity();

		auto id = torus.EmplaceComponent<IdComponent>().id;
		torus.EmplaceComponent<NameComponent>().name = "Torus_" + std::to_string(id);

		torus.EmplaceComponent<TorusComponent>();

		torus.EmplaceComponent<TranslationComponent>();
		torus.EmplaceComponent<RotationComponent>();
		torus.EmplaceComponent<ScalingComponent>();

		torus.EmplaceTag<IsDirtyTag>();

		return torus;
	}

	inline Entity CreatePolyline(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto polyline = scene->CreateEntity();

		auto id = polyline.EmplaceComponent<IdComponent>().id;
		polyline.EmplaceComponent<NameComponent>().name = "Polyline_" + std::to_string(id);

		auto& pc = polyline.EmplaceComponent<LineComponent>();
		pc.pointHandles = pointHandles;
		
		polyline.EmplaceTag<IsDirtyTag>();

		for (auto pointHandle : pointHandles)
		{
			Entity pointEntity{ pointHandle, scene.get() };
			auto& nc = pointEntity.EmplaceComponent<NotificationComponent>();
			nc.AddToNotify(polyline.GetHandle());
		}

		return polyline;
	}

	inline Entity CreateBezierCurveC0(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto curve = scene->CreateEntity();

		auto id = curve.EmplaceComponent<IdComponent>().id;
		curve.EmplaceComponent<NameComponent>().name = "BezierCurveC0_" + std::to_string(id);

		curve.EmplaceTag<IsDirtyTag>();

		auto& bcc = curve.EmplaceComponent<BezierCurveC0Component>();
		
		auto polyline = CreatePolyline(scene, pointHandles);
		polyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		polyline.EmplaceTag<IsInvisibleTag>();

		bcc.polylineHandle = polyline.GetHandle();

		return curve;
	}

	inline Entity CreateBezierCurveC2(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto curve = scene->CreateEntity();

		auto id = curve.EmplaceComponent<IdComponent>().id;
		curve.EmplaceComponent<NameComponent>().name = "BezierCurveC2_" + std::to_string(id);

		curve.EmplaceTag<IsDirtyTag>();

		auto& bcc = curve.EmplaceComponent<BezierCurveC2Component>();

		auto deBoorPolyline = CreatePolyline(scene, pointHandles);
		deBoorPolyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		deBoorPolyline.EmplaceTag<IsInvisibleTag>();

		bcc.deBoorPolylineHandle = deBoorPolyline.GetHandle();

		auto bernsteinPolyline = CreatePolyline(scene, {});
		bernsteinPolyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		bernsteinPolyline.EmplaceTag<IsInvisibleTag>();

		bcc.bernsteinPolylineHandle = bernsteinPolyline.GetHandle();

		return curve;
	}

	inline Entity CreateInterpolatingCurve(Ref<Scene> scene, const std::vector<entt::entity>& pointHandles)
	{
		auto curve = scene->CreateEntity();

		auto id = curve.EmplaceComponent<IdComponent>().id;
		curve.EmplaceComponent<NameComponent>().name = "InterpolatingCurve_" + std::to_string(id);

		curve.EmplaceTag<IsDirtyTag>();

		auto& icc = curve.EmplaceComponent<InterpolatingCurveComponent>();

		auto interpolatingPolyline = CreatePolyline(scene, pointHandles);
		interpolatingPolyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		interpolatingPolyline.EmplaceTag<IsInvisibleTag>();

		icc.interpolatingPolylineHandle = interpolatingPolyline.GetHandle();

		auto bernsteinPolyline = CreatePolyline(scene, {});
		bernsteinPolyline.EmplaceComponent<VirtualComponent>(curve.GetHandle());
		bernsteinPolyline.EmplaceTag<IsInvisibleTag>();

		icc.bernsteinPolylineHandle = bernsteinPolyline.GetHandle();

		return curve;
	}

	inline std::vector<Algebra::Vector4> SetupControlPointsC0(
		BezierSurfaceComponent& bsc, Algebra::Vector4 position, float width, float height)
	{
		int rows = bsc.GetRows();
		int columns = bsc.GetColumns();
		std::vector<Algebra::Vector4> controlPoints;
		controlPoints.reserve(rows * columns);

		switch (bsc.connectionType) 
		{
		case ConnectionType::Flat: 
		{
			float dx = width / static_cast<float>(columns - 1);
			float dy = height / static_cast<float>(rows - 1);
			Algebra::Vector4 startPos = position - Algebra::Vector4(width / 2.f, height / 2.f, 0.f);

			for (int i = 0; i < rows; ++i) 
			{
				for (int j = 0; j < columns; ++j)
				{
					Algebra::Vector4 point = startPos + Algebra::Vector4(j * dx, i * dy, 0.f);
					point.w = 1.f;
					controlPoints.push_back(point);
				}
			}
			break;
		}

		case ConnectionType::Columns: 
		{
			float dHeight = height / static_cast<float>(rows - 1);
			float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(columns - 1);
			Algebra::Vector4 startPos = position - Algebra::Vector4(0.f, 0.f, height / 2.f);

			for (int i = 0; i < rows; ++i) 
			{
				for (int j = 0; j < columns - 1; ++j) 
				{
					Algebra::Vector4 point = startPos + Algebra::Vector4(0.f, 0.f, i * dHeight) +
						Algebra::Matrix4::RotationZ(dAngle * j) *
						Algebra::Vector4(width / 2.f, 0.f, 0.f);
					point.w = 1.f;
					controlPoints.push_back(point);
				}
				controlPoints.push_back(controlPoints[i * columns]);
			}
			break;
		}

		case ConnectionType::Rows: 
		{
			float dHeight = height / static_cast<float>(columns - 1);
			float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(rows - 1);
			Algebra::Vector4 startPos = position - Algebra::Vector4(height / 2.f, 0.f, 0.f);

			for (int i = 0; i < rows - 1; ++i) 
			{
				for (int j = 0; j < columns; ++j) 
				{
					Algebra::Vector4 point = startPos +
						Algebra::Vector4(j * dHeight, 0.f, 0.f) +
						Algebra::Matrix4::RotationX(dAngle * i) *
						Algebra::Vector4(0.f, width / 2.f, 0.f);
					point.w = 1.f;
					controlPoints.push_back(point);
				}
			}
			for (int j = 0; j < columns; ++j)
			{
				controlPoints.push_back(controlPoints[j]);
			}
			break;
		}
		}

		return controlPoints;
	}

	inline std::vector<Algebra::Vector4> UpdatePatchC0(PatchComponent& patch, Ref<Scene> scene) 
	{
		std::vector<Algebra::Vector4> result;
		result.reserve(patch.pointHandles.size());

		for (auto handle : patch.pointHandles) 
		{
			Entity pointEntity{ handle, scene.get() };
			result.push_back(pointEntity.GetComponent<TranslationComponent>().translation);
		}

		return result;
	}

	inline void InitializePatch(PatchComponent& pc, Ref<Scene> scene, const std::vector<entt::entity>& handles) 
	{
		pc.pointHandles = handles;
		pc.onUpdate = [scene](PatchComponent& patch) {
			return UpdatePatchC0(patch, scene);
			};
	}

	inline Entity CreateBezierSurfaceC0(Ref<Scene> scene) {
		auto surface = scene->CreateEntity();

		// ID & Name
		auto id = surface.EmplaceComponent<IdComponent>().id;
		surface.EmplaceComponent<NameComponent>().name = "BezierSurfaceC0_" + std::to_string(id);
		surface.EmplaceTag<IsDirtyTag>();

		auto& bsc = surface.EmplaceComponent<BezierSurfaceComponent>();
		auto controlPoints = SetupControlPointsC0(bsc, Algebra::Vector4(0.f, 0.f, 0.f), 4.f, 4.f);

		// Create points
		bsc.pointHandles.reserve(controlPoints.size());
		for (size_t i = 0; i < controlPoints.size(); ++i) 
		{
			Entity point = ShapeCreator::CreatePoint(scene);
			point.GetComponent<TranslationComponent>().SetTranslation(controlPoints[i]);
			point.EmplaceTag<IsInvisibleTag>();

			bsc.pointHandles.push_back(point.GetHandle());
			auto& nc = point.EmplaceComponent<NotificationComponent>();
			nc.AddToNotify(surface.GetHandle());
		}

		// Create patches
		bsc.patchHandles.reserve(bsc.widthPatches * bsc.heightPatches);
		for (int patchIndex = 0; patchIndex < bsc.widthPatches * bsc.heightPatches; ++patchIndex) 
		{
			Entity patchEntity = scene->CreateEntity();
			auto& pc = patchEntity.EmplaceComponent<PatchComponent>();
			patchEntity.EmplaceComponent<VirtualComponent>(surface.GetHandle());

			int startI = patchIndex / bsc.widthPatches;
			int startJ = patchIndex % bsc.widthPatches;

			std::vector<entt::entity> handles;
			handles.reserve(16);
			for (int i = 0; i < 4; ++i) 
			{
				for (int j = 0; j < 4; ++j) 
				{
					handles.push_back(bsc.pointHandles[(startI * 3 + i) * bsc.GetColumns() + startJ * 3 + j]);
				}
			}

			InitializePatch(pc, scene, handles);
			bsc.patchHandles.push_back(patchEntity.GetHandle());
		}

		return surface;
	}

	inline Entity CreateBezierSurfaceC2(Ref<Scene> scene)
	{
		auto surface = scene->CreateEntity();

		auto id = surface.EmplaceComponent<IdComponent>().id;
		surface.EmplaceComponent<NameComponent>().name = "BezierSurfaceC2_" + std::to_string(id);

		surface.EmplaceTag<IsDirtyTag>();

		auto& bsc = surface.EmplaceComponent<BezierSurfaceC2Component>();
		for (int i = 0; i < bsc.GetColumns() * bsc.GetRows(); ++i)
		{
			Entity pointEntity = ShapeCreator::CreatePoint(scene);
			pointEntity.EmplaceTag<IsInvisibleTag>();
			bsc.pointHandles.push_back(pointEntity.GetHandle());
			// Probably here
			auto& nc = pointEntity.EmplaceComponent<NotificationComponent>();
			nc.AddToNotify(surface.GetHandle());
		}

		for (int patchIndex = 0; patchIndex < bsc.widthPatches * bsc.heightPatches; ++patchIndex)
		{
			Entity patchEntity = scene->CreateEntity();
			auto& pc = patchEntity.EmplaceComponent<PatchComponent>();
			patchEntity.EmplaceComponent<VirtualComponent>(surface.GetHandle());

			int startingI = patchIndex / bsc.widthPatches;
			int startingJ = patchIndex % bsc.widthPatches;

			std::vector<entt::entity> controlPoints;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					controlPoints.push_back(bsc.pointHandles[(startingI + i) * bsc.GetColumns() + startingJ + j]);
				}
			}

			pc.pointHandles = controlPoints;
			pc.deBoorPolylineHandle = ShapeCreator::CreatePolyline(scene, controlPoints).GetHandle();
			pc.bernsteinPolylineHandle = ShapeCreator::CreatePolyline(scene, {}).GetHandle();
			bsc.patchHandles.push_back(patchEntity.GetHandle());
		}

		return surface;
	}

}