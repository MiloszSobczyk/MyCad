#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
	GLCall(glGenBuffers(1, &id));
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}