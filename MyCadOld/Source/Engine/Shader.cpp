#include "Shader.h"
#include "Utils/Logger.h"
#include <GL/glew.h>

Shader::Shader(unsigned int programID)
    : m_RendererID(programID) 
{
}

Shader::~Shader() 
{
    glDeleteProgram(m_RendererID);
}

void Shader::Bind() const 
{
    GLCall(glUseProgram(m_RendererID));
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
        return m_UniformLocationCache[name];

    GLCall(unsigned int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1)
        printf("Warning: uniform '%s' doesn't exist!\n", name.c_str());

    m_UniformLocationCache[name] = location;
    return location;
}
