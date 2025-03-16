#pragma once
#include <vector>
#include <Utils/Logger.h>
#include "VertexLayout.h"
#include <GL/glew.h>

class VertexBuffer
{
private:
    unsigned int count;
    unsigned int id;

public:
    VertexBuffer();
    template <typename T> VertexBuffer(const std::vector<T>& vertices);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;

    unsigned int GetCount() const;
    template <typename T> void SetVertices(const std::vector<T>& vertices);
};

template <typename T>
VertexBuffer::VertexBuffer(const std::vector<T>& vertices)
    : VertexBuffer()
{
    SetVertices(vertices);
}

template <typename T>
void VertexBuffer::SetVertices(const std::vector<T>& vertices)
{
    count = static_cast<unsigned int>(vertices.size());
    Bind();
    GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_DYNAMIC_DRAW));
    Unbind();
}