#include "BufferLayout.h"

void BufferLayout::CalculateOffsetsAndStride()
{
	size_t offset = 0;
	m_Stride = 0;
	for (auto& element : m_Elements)
	{
		element.Offset = offset;
		offset += element.Size;
		m_Stride += element.Size;
	}
}
