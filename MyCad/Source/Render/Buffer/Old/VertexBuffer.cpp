#include "VertexBuffer.h"

VertexBuffer::VertexBuffer() 
{
    glGenBuffers(1, &id);
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