#include "IdManager.h"

IdManager& IdManager::GetInstance()
{
    static IdManager instance;

    return instance;
}

unsigned int IdManager::GetNextId()
{
    // TODO: freeing IDs needs to be readded
    //if (!m_FreeIds.empty())
    //{
    //    unsigned int id = m_FreeIds.front();
    //    m_FreeIds.pop();
    //    return id;
    //}
    return m_CurrentId++;
}

void IdManager::FreeId(unsigned int id)
{
    m_FreeIds.push(id);
}