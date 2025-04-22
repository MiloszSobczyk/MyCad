#pragma once

#include "Matrix4.h"
#include "Vector4.h"

#include <string>
#include <unordered_map>

class Shader 
{
public:
    Shader(unsigned int programID);
    ~Shader();

    void Bind() const;
    void Unbind() const;

    void SetUniformMat4f(const std::string& name, const Algebra::Matrix4& matrix);
    void SetUniformVec4f(const std::string& name, const Algebra::Vector4& vector);

private:
    unsigned int m_RendererID;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;

    int GetUniformLocation(const std::string& name);
};
