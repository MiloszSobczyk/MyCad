#pragma once

#include "Core/Scene/Scene.h"
#include "Utils/Base.h"

class  NotificationSystem
{
public:
	NotificationSystem(Ref<Scene> scene);

	void Update();

private:
	Ref<Scene> m_Scene;
};