#pragma once

#include <GL/glew.h>

#include <vector>

class IndexBuffer 
{
public:
    IndexBuffer();
    IndexBuffer(const std::vector<unsigned int>& indices);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;
    void SetIndices(const std::vector<unsigned int>& indices);

    inline GLuint GetID() const { return id; }
    inline unsigned int GetCount() const { return count; }

private:
    GLuint id = 0;
    unsigned int count = 0;
};
