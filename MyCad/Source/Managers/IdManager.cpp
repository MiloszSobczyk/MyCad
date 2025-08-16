#include "IdManager.h"

IdManager::IdManager()
{

}

IdManager::~IdManager()
{
}

IdManager& IdManager::GetInstance()
{
    static IdManager instance;

    return instance;
}

unsigned int IdManager::GetNextId()
{
    return m_CurrentId++;
}
