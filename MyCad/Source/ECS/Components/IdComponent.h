#pragma once

#include "Managers/IdManager.h"

struct IdComponent
{
	unsigned int id;

	IdComponent()
		: id{ IdManager::GetInstance().GetNextId() }
	{}

	~IdComponent()
	{
		IdManager::GetInstance().FreeId(id);
	}
};