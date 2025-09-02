#pragma once

#include <entt/entt.hpp>

struct DirtyFromComponent
{
	entt::entity sourceEntity;

	DirtyFromComponent() = default;
	DirtyFromComponent(entt::entity e) : sourceEntity(e) {}
};