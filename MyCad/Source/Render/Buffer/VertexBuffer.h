#pragma once

#include <GL/glew.h>

#include <cstddef>

class VertexBuffer 
{
public:
    VertexBuffer();
    VertexBuffer(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
    void SetData(const void* data, size_t size, GLenum usage = GL_STATIC_DRAW);

    inline GLuint GetID() const { return id; }

private:
    GLuint id = 0;
};