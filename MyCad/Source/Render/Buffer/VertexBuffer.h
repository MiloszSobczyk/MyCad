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

	const BufferLayout& GetLayout() const { return m_Layout; }
	void SetLayout(const BufferLayout& layout) { m_Layout = layout; }

private:
	uint32_t m_RendererId = 0;
	BufferLayout m_Layout;
};