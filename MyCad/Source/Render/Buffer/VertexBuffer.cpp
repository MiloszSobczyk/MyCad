#include "VertexBuffer.h"

#include <GL/glew.h>


VertexBuffer::VertexBuffer(uint32_t size)
{
	glCreateBuffers(1, &m_RendererId);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(const void* data, uint32_t size)
{
	glCreateBuffers(1, &m_RendererId);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &m_RendererId);
}

void VertexBuffer::Bind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
}

void VertexBuffer::Unbind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void* data, uint32_t size)
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererId);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

