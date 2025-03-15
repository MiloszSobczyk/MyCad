#pragma once
#include <vector>
#include <Utils/Logger.h>
#include "VertexLayout.h"
#include <GL/glew.h>

class VertexBuffer
{
private:
    unsigned int id;

public:
    VertexBuffer();

    template <typename T>
    VertexBuffer(const std::vector<T>& vertices)
    {
        GLCall(glGenBuffers(1, &id));
        SetVertices(vertices);
    }

    void Bind() const;
    void Unbind() const;

    template <typename T>
    void SetVertices(const std::vector<T>& vertices) const
    {
        Bind();
        GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_DYNAMIC_DRAW));
        Unbind();
    }
};
