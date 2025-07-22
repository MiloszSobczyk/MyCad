#include "Shader.h"

#include "Utils/Logger.h"

#include <GL/glew.h>

#include <iostream>

Shader::Shader(unsigned int programID)
    : m_ProgramID{ programID }
{
}

Shader::~Shader() 
{
    GLCall(glDeleteProgram(m_ProgramID));
}

void Shader::Bind() const 
{
    GLCall(glUseProgram(m_ProgramID));
}

void Shader::Unbind() const 
{
    GLCall(glUseProgram(0));
}

void Shader::SetUniformMat4f(const std::string& name, const Algebra::Matrix4& matrix) 
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_TRUE, &matrix[0][0]));
}

void Shader::SetUniformVec4f(const std::string& name, const Algebra::Vector4& vector) 
{
    GLCall(glUniform4f(GetUniformLocation(name), vector.x, vector.y, vector.z, vector.w));
}

void Shader::SetUniformInt(const std::string& name, int value)
{
    GLCall(glUniform1i(GetUniformLocation(name), value));
}

int Shader::GetUniformLocation(const std::string& name) 
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
    {
        return m_UniformLocationCache[name];
    }

    GLCall(unsigned int location = glGetUniformLocation(m_ProgramID, name.c_str()));
    if (location == -1)
    {
		std::cerr << "Warning: uniform '" << name << "' doesn't exist in shader program!\n";
    }

    m_UniformLocationCache[name] = location;
    return location;
}
