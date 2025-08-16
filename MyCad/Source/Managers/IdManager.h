#pragma once

class IdManager
{
public:
	static IdManager& GetInstance();

	unsigned int GetNextId();

private:
	IdManager();
	~IdManager();
	IdManager(const IdManager&) = delete;
	void operator=(const IdManager&) = delete;

	unsigned int m_CurrentId = 0;
};