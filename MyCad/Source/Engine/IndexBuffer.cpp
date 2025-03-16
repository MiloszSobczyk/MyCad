#include "IndexBuffer.h"

IndexBuffer::IndexBuffer()
	: id(0), count(0)
{
	GLCall(glGenBuffers(1, &id));
}

IndexBuffer::IndexBuffer(std::vector<unsigned int> indices)
	: IndexBuffer()
{
	SetIndices(indices);
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &id));
}

unsigned int IndexBuffer::GetCount() const
{
	return count;
}

void IndexBuffer::SetIndices(std::vector<unsigned int> indices)
{
	count = static_cast<unsigned int>(indices.size());
	Bind();
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_DYNAMIC_DRAW));
	Unbind();
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id));
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}
