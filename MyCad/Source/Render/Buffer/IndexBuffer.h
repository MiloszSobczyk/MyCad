#pragma once

#include <cstdint>

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* indices, uint32_t count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	uint32_t GetCount() const;

private:
	uint32_t m_RendererID = 0;
	uint32_t m_Count = 0;
};