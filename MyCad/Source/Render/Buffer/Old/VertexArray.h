#pragma once

#include <GL/glew.h>

class VertexArray 
{
public:
    VertexArray();
    ~VertexArray();

    void Bind() const;
    void Unbind() const;

    inline GLuint GetID() const { return id; }

private:
    GLuint id = 0;
};
