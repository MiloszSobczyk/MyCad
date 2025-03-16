#pragma once
#include "Utils/Logger.h"
#include <vector>

class IndexBuffer
{
private:
	unsigned int count;
	unsigned int id;

public:
	IndexBuffer();
	IndexBuffer(std::vector<unsigned int> indices);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	unsigned int GetCount() const;
	void SetIndices(std::vector<unsigned int> indices);
};
