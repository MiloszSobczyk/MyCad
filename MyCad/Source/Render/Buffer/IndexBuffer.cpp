#include "IndexBuffer.h"

IndexBuffer::IndexBuffer() 
{
    glGenBuffers(1, &id);
}

IndexBuffer::IndexBuffer(const std::vector<unsigned int>& indices) 
{
    glGenBuffers(1, &id);
    SetIndices(indices);
}

IndexBuffer::~IndexBuffer() 
{
    glDeleteBuffers(1, &id);
}

void IndexBuffer::Bind() const 
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
}

void IndexBuffer::Unbind() const 
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::SetIndices(const std::vector<unsigned int>& indices) 
{
    count = static_cast<unsigned int>(indices.size());
    Bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
}