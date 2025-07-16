#pragma once

#include <GL/glew.h>

#include <cstddef>
#include <vector>

class VertexBuffer 
{
public:
    VertexBuffer();
    template<class T> VertexBuffer(std::vector<T> vertices, GLenum usage = GL_DYNAMIC_DRAW);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
    template<class T> void SetVertices(std::vector<T> vertices, GLenum usage = GL_DYNAMIC_DRAW);

    inline GLuint GetID() const { return id; }
    inline unsigned int GetCount() const { return count; }

private:
    GLuint id = 0;
    unsigned int count = 0;
};

template<class T>
inline VertexBuffer::VertexBuffer(std::vector<T> vertices, GLenum usage)
    : VertexBuffer()
{
    glGenBuffers(1, &id);
    SetVertices(vertices, usage);
}

template<class T>
inline void VertexBuffer::SetVertices(std::vector<T> vertices, GLenum usage)
{
    count = vertices.size();
    Bind();
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), usage);
    Unbind();
}
