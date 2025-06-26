#pragma once

#include <vector>

#include "utils/Logger.h"

class IndexBuffer
{
private:
	unsigned int id;
	int count;
public:
	IndexBuffer();
	IndexBuffer(std::vector<unsigned int> indices);
	~IndexBuffer();

	int GetCount() const;

	void SetIndices(std::vector<unsigned int> indices);
	
	void Bind() const;
	void Unbind() const;
};

