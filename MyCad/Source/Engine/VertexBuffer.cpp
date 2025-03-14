#include "VertexBuffer.h"

VertexBuffer::VertexBuffer()
{
	GLCall(glGenBuffers(1, &id));
}

VertexBuffer::VertexBuffer(std::vector<PositionColorVertex> vertices)
{
	GLCall(glGenBuffers(1, &id));
	SetVertices(vertices);
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, id));
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::SetVertices(std::vector<PositionColorVertex> vertices) const
{
	Bind();
	GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(PositionColorVertex), vertices.data(), GL_DYNAMIC_DRAW));
	Unbind();
}