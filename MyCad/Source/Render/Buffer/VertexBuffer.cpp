#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() 
{
    glGenBuffers(1, &id);
}

VertexBuffer::VertexBuffer(const void* data, size_t size, GLenum usage) 
{
    glGenBuffers(1, &id);
    SetData(data, size, usage);
}

VertexBuffer::~VertexBuffer() 
{
    glDeleteBuffers(1, &id);
}

void VertexBuffer::Bind() const 
{
    glBindBuffer(GL_ARRAY_BUFFER, id);
}

void VertexBuffer::Unbind() const 
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::SetData(const void* data, size_t size, GLenum usage) 
{
    Bind();
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
}