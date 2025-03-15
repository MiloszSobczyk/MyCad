#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
	: count(0), id(0)
{
	GLCall(glGenBuffers(1, &id));
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &id));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

int VertexBuffer::GetCount() const
{
	return 0;
}
