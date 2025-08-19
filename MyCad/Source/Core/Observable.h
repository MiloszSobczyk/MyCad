#pragma once

#include "Scene/Entity.h"
#include "ECS/Components/Tags.h"

template<typename T>
class Observable
{
public:
	T value;
	Entity entity;

	void Set(const T& newValue)
	{
		if (value != newValue)
		{
			value = newValue;
			entity.EmplaceTag<IsNotifiedTag>();
		}
	}

	operator const T& () const { return value; }
};