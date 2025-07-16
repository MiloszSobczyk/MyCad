#include "VertexArray.h"

VertexArray::VertexArray() 
{
    glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray() 
{
    glDeleteVertexArrays(1, &id);
}

void VertexArray::Bind() const 
{
    glBindVertexArray(id);
}

void VertexArray::Unbind() const 
{
    glBindVertexArray(0);
}