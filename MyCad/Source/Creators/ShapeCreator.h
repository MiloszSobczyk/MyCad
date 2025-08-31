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

		auto& pc = polyline.EmplaceComponent<PolylineComponent>();
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




	inline std::vector<Algebra::Vector4> SetupControlPoints(
		BezierSurfaceComponent& bsc, Algebra::Vector4 position, float width, float height)
	{
		int rows = bsc.GetRows();
		int columns = bsc.GetColumns();
		bool c2 = bsc.C2;
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
				for (int j = 0; j < columns - (c2 ? 3 : 1); ++j) 
				{
					Algebra::Vector4 point = startPos + Algebra::Vector4(0.f, 0.f, i * dHeight) +
						Algebra::Matrix4::RotationZ(dAngle * j) *
						Algebra::Vector4(width / 2.f, 0.f, 0.f);
					point.w = 1.f;
					controlPoints.push_back(point);
				}
				controlPoints.push_back(controlPoints[i * columns + 0]);
				if (c2)
				{
					controlPoints.push_back(controlPoints[i * columns + 1]);
					controlPoints.push_back(controlPoints[i * columns + 2]);
				}
			}
			break;
		}

		case ConnectionType::Rows: 
		{
			float dHeight = height / static_cast<float>(columns - 1);
			float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(rows - 1);
			Algebra::Vector4 startPos = position - Algebra::Vector4(height / 2.f, 0.f, 0.f);

			for (int i = 0; i < rows - 3; ++i) 
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
			for (int i = 0; i < (c2 ? 3 : 1); ++i)
			{
				for (int j = 0; j < columns; ++j)
				{
					controlPoints.push_back(controlPoints[i * columns + j]);
				}
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

	inline void InitializePatchPolylinePoints(Entity polylineEntity, std::vector<entt::entity> pointHandles,
		Ref<Scene> scene, bool addToNotify = true)
	{
		std::vector<entt::entity> polylinePointHandles = {
			// Rows
			pointHandles[0],
			pointHandles[1],
			pointHandles[2],
			pointHandles[3],

			pointHandles[7],
			pointHandles[6],
			pointHandles[5],
			pointHandles[4],

			pointHandles[8],
			pointHandles[9],
			pointHandles[10],
			pointHandles[11],

			pointHandles[15],
			pointHandles[14],
			pointHandles[13],
			pointHandles[12],

			// Columns
			pointHandles[12],
			pointHandles[8],
			pointHandles[4],
			pointHandles[0],

			pointHandles[1],
			pointHandles[5],
			pointHandles[9],
			pointHandles[13],

			pointHandles[14],
			pointHandles[10],
			pointHandles[6],
			pointHandles[2],

			pointHandles[3],
			pointHandles[7],
			pointHandles[11],
			pointHandles[15]
		};

		auto& polylineComponent = polylineEntity.GetComponent<PolylineComponent>();
		polylineComponent.pointHandles = polylinePointHandles;
		if (addToNotify)
		{
			for(auto handle : polylinePointHandles)
			{
				Entity pointEntity{ handle, scene.get() };
				auto& notificationComponent = pointEntity.GetComponent<NotificationComponent>();
				notificationComponent.AddToNotify(polylineEntity.GetHandle());
			}
		}
	}

	inline Entity CreateBezierSurfaceC0(Ref<Scene> scene) 
	{
		auto surface = scene->CreateEntity();

		// ID & Name
		auto id = surface.EmplaceComponent<IdComponent>().id;
		surface.EmplaceComponent<NameComponent>().name = "BezierSurfaceC0_" + std::to_string(id);
		surface.EmplaceTag<IsDirtyTag>();

		auto& bsc = surface.EmplaceComponent<BezierSurfaceComponent>();
		auto controlPoints = SetupControlPoints(bsc, Algebra::Vector4(0.f, 0.f, 0.f), 4.f, 4.f);

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
			patchEntity.EmplaceTag<IsDirtyTag>();
			patchEntity.EmplaceComponent<VirtualComponent>(surface.GetHandle());
			
			auto& patchComponent = patchEntity.EmplaceComponent<PatchComponent>();
			patchComponent.bernsteinPolylineHandle = CreatePolyline(scene, {}).GetHandle();

			int startI = patchIndex / bsc.widthPatches;
			int startJ = patchIndex % bsc.widthPatches;

			std::vector<entt::entity> handles;
			handles.reserve(16);
			for (int i = 0; i < 4; ++i) 
			{
				for (int j = 0; j < 4; ++j) 
				{
					auto pointHandle = bsc.pointHandles[(startI * 3 + i) * bsc.GetColumns() + startJ * 3 + j];
					auto pointEntity = Entity{ pointHandle, scene.get() };
					pointEntity.GetComponent<NotificationComponent>().AddToNotify(patchEntity.GetHandle());

					handles.push_back(pointHandle);
				}
			}

			patchComponent.pointHandles = handles;
			patchComponent.onUpdate = [scene](PatchComponent& patch) {
				return UpdatePatchC0(patch, scene);
				};
			InitializePatchPolylinePoints(Entity{ patchComponent.bernsteinPolylineHandle, scene.get() }, handles, scene);

			bsc.patchHandles.push_back(patchEntity.GetHandle());
		}

		return surface;
	}

	inline std::vector<Algebra::Vector4> UpdatePatchC2(PatchComponent& patch, Ref<Scene> scene, 
		PolylineComponent& bernsteinPolyline)
	{
		static constexpr float A[4][4] = {
			{ 1.f / 6.f, 4.f / 6.f, 1.f / 6.f, 0.f },
			{ 0.f,       4.f / 6.f, 2.f / 6.f, 0.f },
			{ 0.f,       2.f / 6.f, 4.f / 6.f, 0.f },
			{ 0.f,       1.f / 6.f, 4.f / 6.f, 1.f / 6.f }
		};

		std::array<std::array<Algebra::Vector4, 4>, 4> P;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				P[i][j] = Entity{ patch.pointHandles[i * 4 + j], scene.get() }
					.GetComponent<TranslationComponent>().translation;
			}
		}

		std::array<std::array<Algebra::Vector4, 4>, 4> Q{};
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				for (int k = 0; k < 4; ++k)
				{
					Q[i][j] += A[i][k] * P[k][j];
				}
			}
		}

		std::vector<Algebra::Vector4> bernsteinPoints;
		bernsteinPoints.reserve(16);

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				Algebra::Vector4 b;
				for (int k = 0; k < 4; ++k)
				{
					b += Q[i][k] * A[j][k];
				}
				bernsteinPoints.push_back(b);
			}
		}

		for(int i = 0; i < 16; ++i)
		{
			Entity point{ bernsteinPolyline.pointHandles[i], scene.get() };
			point.GetComponent<TranslationComponent>().SetTranslation(bernsteinPoints[i]);
			point.RemoveComponent<IsNotifiedTag>();
		}

		return bernsteinPoints;
	}

	inline Entity CreateBezierSurfaceC2(Ref<Scene> scene)
	{
		auto surface = scene->CreateEntity();

		// ID & Name
		auto id = surface.EmplaceComponent<IdComponent>().id;
		surface.EmplaceComponent<NameComponent>().name = "BezierSurfaceC2_" + std::to_string(id);
		surface.EmplaceTag<IsDirtyTag>();

		auto& bsc = surface.EmplaceComponent<BezierSurfaceComponent>();
		bsc.C2 = true;
		auto controlPoints = SetupControlPoints(bsc, Algebra::Vector4(0.f, 0.f, 0.f), 4.f, 4.f);

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
			patchEntity.EmplaceTag<IsDirtyTag>();
			patchEntity.EmplaceComponent<VirtualComponent>(surface.GetHandle());

			auto& patchComponent = patchEntity.EmplaceComponent<PatchComponent>();
			patchComponent.deBoorPolylineHandle = CreatePolyline(scene, {}).GetHandle();
			patchComponent.bernsteinPolylineHandle = CreatePolyline(scene, {}).GetHandle();

			int startI = patchIndex / bsc.widthPatches;
			int startJ = patchIndex % bsc.widthPatches;

			std::vector<entt::entity> deBoorPointHandles;
			deBoorPointHandles.reserve(16);
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					auto pointHandle = bsc.pointHandles[(startI + i) * bsc.GetColumns() + startJ + j];
					auto pointEntity = Entity{ pointHandle, scene.get() };
					pointEntity.GetComponent<NotificationComponent>().AddToNotify(patchEntity.GetHandle());

					deBoorPointHandles.push_back(pointHandle);
				}
			}

			patchComponent.pointHandles = deBoorPointHandles;

			auto& bernsteinPolyline = Entity{ patchComponent.bernsteinPolylineHandle, scene.get() }.GetComponent<PolylineComponent>();
			patchComponent.onUpdate = [scene, &bernsteinPolyline](PatchComponent& patch) {
				return UpdatePatchC2(patch, scene, bernsteinPolyline);
				};
			InitializePatchPolylinePoints(Entity{ patchComponent.deBoorPolylineHandle, scene.get() }, deBoorPointHandles, scene);

			auto bernsteinPointHandles = std::vector<entt::entity>{};
			bernsteinPointHandles.reserve(16);
			for (int i = 0; i < 16; ++i)
			{
				Entity point = ShapeCreator::CreatePoint(scene);
				point.EmplaceTag<IsInvisibleTag>();

				bernsteinPointHandles.push_back(point.GetHandle());
			}
			InitializePatchPolylinePoints(Entity{ patchComponent.bernsteinPolylineHandle, scene.get() }, bernsteinPointHandles, scene, false);

			bsc.patchHandles.push_back(patchEntity.GetHandle());
		}

		return surface;
	}
}