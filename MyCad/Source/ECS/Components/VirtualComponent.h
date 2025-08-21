#pragma once

#include <entt/entt.hpp>

struct VirtualComponent
{
	entt::entity targetEntity;
	// Perhaps it could replace IsInvisibleTag
	// bool isVisible = true;

	VirtualComponent() = default;
	VirtualComponent(entt::entity e) : targetEntity(e) {}
};;