#pragma once

#include <entt/entt.hpp>

struct VirtualComponent
{
	entt::entity targetEntity;

	VirtualComponent() = default;
	VirtualComponent(entt::entity e) : targetEntity(e) {}
};