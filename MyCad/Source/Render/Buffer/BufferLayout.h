#pragma once 

#include "BufferElement.h"

#include <vector>


class BufferLayout
{
public:
	BufferLayout() {}

	// Takes an initializer list of BufferElements to define the layout
	BufferLayout(std::initializer_list<BufferElement> elements)
		: m_Elements(elements)
	{
		CalculateOffsetsAndStride();
	}

	uint32_t GetStride() const { return m_Stride; } 
	const std::vector<BufferElement>& GetElements() const { return m_Elements; }

	std::vector<BufferElement>::iterator begin() { return m_Elements.begin(); }
	std::vector<BufferElement>::iterator end() { return m_Elements.end(); }
	std::vector<BufferElement>::const_iterator begin() const { return m_Elements.begin(); }
	std::vector<BufferElement>::const_iterator end() const { return m_Elements.end(); }

private:
	// Calculates the offset of each attribute and total stride
	void CalculateOffsetsAndStride();

private:
	std::vector<BufferElement> m_Elements;
	uint32_t m_Stride = 0; // Stride = total size of one vertex
};
