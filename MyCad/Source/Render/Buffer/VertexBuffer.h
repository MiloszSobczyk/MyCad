#pragma once

#include "Utils/Base.h"
#include "BufferLayout.h"

#include <cstdint>

class VertexBuffer
{
public:
	VertexBuffer(uint32_t size);
	VertexBuffer(const void* data, uint32_t size);
	~VertexBuffer();

	void Bind() const;
	void Unbind() const;

	void SetData(const void* data, uint32_t size);

	inline const BufferLayout& GetLayout() const { return m_Layout; }
	inline void SetLayout(const BufferLayout& layout) { m_Layout = layout; }
	inline uint32_t GetVertexCount() const
	{
		if (m_Layout.GetStride() == 0)
			return 0;
		return m_Size / m_Layout.GetStride();
	}

private:
	uint32_t m_RendererId = 0;
	uint32_t m_Size = 0;
	BufferLayout m_Layout;
};