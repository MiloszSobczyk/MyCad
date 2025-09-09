#pragma once

#include "Render/Uniform/Uniform.h"

#include "Algebra.h"

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
    void SetUniformInt(const std::string& name, int value);

    const std::unordered_map<std::string, UniformDefinition>& GetUniformDefinitions() const
    {
        return m_UniformDefinitions;
    }

private:
    void InitializeUniforms();
    int GetUniformLocation(const std::string& name);

private:
    unsigned int m_ProgramID;
    mutable std::unordered_map<std::string, int> m_UniformLocationCache;
    std::unordered_map<std::string, UniformDefinition> m_UniformDefinitions;
};
