#include "ShapeManagementSystem.h"

#include "ECS/Components/Components.h"

#include "Creators/ShapeCreator.h"

ShapeManagementSystem::ShapeManagementSystem(Ref<Scene> scene)
	: m_Scene{ scene }
{
}

void ShapeManagementSystem::Update()
{
	MergePoints();
}

// https://www.youtube.com/watch?v=mWMFTfaJaWM
void ShapeManagementSystem::MergePoints()
{
	auto mergedPoints = m_Scene->GetAllEntitiesWith<ToBeMergedTag>();
	
	if (mergedPoints.size() < 2)
		return;

	Algebra::Vector4 averagePosition = { 0.f, 0.f, 0.f, 0.f };

	for (auto point : mergedPoints)
	{
		averagePosition += point.GetComponent<TranslationComponent>().translation;
	}

	averagePosition = {
		averagePosition.x / mergedPoints.size(),
		averagePosition.y / mergedPoints.size(),
		averagePosition.z / mergedPoints.size(),
		1.0f
	};

	auto newPoint = ShapeCreator::CreatePoint(m_Scene);
	newPoint.GetComponent<TranslationComponent>().SetTranslation(averagePosition);
	newPoint.EmplaceComponent<NotificationComponent>();

	for (auto point : mergedPoints)
	{
		if (!point.HasComponent<NotificationComponent>())
		{
			m_Scene->DestroyEntity(point);
			continue;
		}

		auto& nc = point.GetComponent<NotificationComponent>();
		
		for (auto toNotify : nc.toNotifys)
		{
			Entity toNotifyEntity{ toNotify, m_Scene.get() };
			newPoint.EmplaceComponent<NotificationComponent>()
				.AddToNotify(toNotifyEntity.GetHandle());

			if (toNotifyEntity.HasComponent<PatchComponent>())
			{
				auto& pc = toNotifyEntity.GetComponent<PatchComponent>();
				for (auto& handle : pc.pointHandles)
				{
					if (handle == point.GetHandle())
					{
						handle = newPoint.GetHandle();
					}
				}
			}
			else if (toNotifyEntity.HasComponent<PolylineComponent>())
			{
				auto& lc = toNotifyEntity.GetComponent<PolylineComponent>();
				for (auto& handle : lc.pointHandles)
				{
					if (handle == point.GetHandle())
					{
						handle = newPoint.GetHandle();
					}
				}
			}
			else if (toNotifyEntity.HasComponent<CurveComponent>())
			{
				auto& cc = toNotifyEntity.GetComponent<CurveComponent>();
				auto& controlPolyline = Entity{ cc.controlPolylineHandle, m_Scene.get() }
					.GetComponent<PolylineComponent>();
				for (auto& handle : controlPolyline.pointHandles)
				{
					if (handle == point.GetHandle())
					{
						handle = newPoint.GetHandle();
					}
				}
			}
			else if (toNotifyEntity.HasComponent<BezierSurfaceComponent>())
			{
				auto& bsc = toNotifyEntity.GetComponent<BezierSurfaceComponent>();
				for (auto& handle : bsc.pointHandles)
				{
					if (handle == point.GetHandle())
					{
						handle = newPoint.GetHandle();
					}
				}
			}
			toNotifyEntity.EmplaceComponent<DirtyFromComponent>();

		}
		nc.ClearToNotify();
		m_Scene->DestroyEntity(point);
	}
}