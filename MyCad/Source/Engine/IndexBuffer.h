#pragma once
#include "Utils/Logger.h"
#include <vector>

class IndexBuffer
{
private:
	int count;
	unsigned int id;

public:
	IndexBuffer();
	IndexBuffer(std::vector<unsigned int> indices);
	~IndexBuffer();

	int GetCount() const;
	void SetIndices(std::vector<unsigned int> indices);

	void Bind() const;
	void Unbind() const;
};
