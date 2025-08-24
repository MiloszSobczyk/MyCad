

#pragma once

#include "Core/Scene/Scene.h"
#include "Utils/Base.h"

class  NotificationSystem
{
public:
	NotificationSystem(Ref<Scene> scene);
	~NotificationSystem() = default;

	NotificationSystem(const NotificationSystem&) = delete;
	NotificationSystem& operator=(const NotificationSystem&) = delete;

	void Update();

private:
	Ref<Scene> m_Scene;
};