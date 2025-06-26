#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
	: count(0), id(0)
{
	GLCall(glGenBuffers(1, &id));
}

VertexBuffer::~VertexBuffer()
{
	glDeleteBuffers(1, &id);
}

int VertexBuffer::GetCount() const
{
	return count;
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
