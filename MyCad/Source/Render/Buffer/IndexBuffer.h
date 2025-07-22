#pragma once

#include <cstdint>

class IndexBuffer
{
public:
	IndexBuffer(uint32_t* indices, uint32_t count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline uint32_t GetCount() const { return this->m_Count; }

private:
	uint32_t m_RendererId = 0;
	uint32_t m_Count = 0;
};