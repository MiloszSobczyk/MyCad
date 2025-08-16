#pragma once

#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

#include <string>

namespace UI
{
	inline std::string GenerateLabel(Entity entity, std::string name)
	{
		return name + "##" + std::to_string(entity.GetId());
	}
}