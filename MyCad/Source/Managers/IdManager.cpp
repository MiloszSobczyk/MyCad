#include "IdManager.h"

IdManager& IdManager::GetInstance()
{
    static IdManager instance;

    return instance;
}

unsigned int IdManager::GetNextId()
{
    return m_CurrentId++;
}

void IdManager::FreeId(unsigned int id)
{
    m_FreeIds.push(id);
}