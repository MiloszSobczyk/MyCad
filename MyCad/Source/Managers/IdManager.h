#pragma once

#include <queue>

class IdManager
{
public:
    static IdManager& GetInstance();

    unsigned int GetNextId();
    void FreeId(unsigned int id);

private:
    IdManager() = default;
    ~IdManager() = default;
    IdManager(const IdManager&) = delete;
    void operator=(const IdManager&) = delete;

    unsigned int m_CurrentId = 0;
    std::queue<unsigned int> m_FreeIds;
};