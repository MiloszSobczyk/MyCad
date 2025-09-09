#include "Shader.h"

#include "Utils/Logger.h"

#include <GL/glew.h>

#include <iostream>

Shader::Shader(unsigned int programID)
    : m_ProgramID{ programID }
{
    InitializeUniforms();
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

void Shader::InitializeUniforms()
{
    m_UniformDefinitions.clear();

    GLint uniformCount = 0;
    glGetProgramiv(m_ProgramID, GL_ACTIVE_UNIFORMS, &uniformCount);

    for (GLint i = 0; i < uniformCount; i++)
    {
        char nameBuffer[256];
        GLsizei length;
        GLint size;
        GLenum type;

        glGetActiveUniform(m_ProgramID, i, sizeof(nameBuffer), &length, &size, &type, nameBuffer);
        std::string name(nameBuffer, length);

        int location = glGetUniformLocation(m_ProgramID, name.c_str());

        UniformDefinition def;
        def.name = name;
        def.type = ToUniformType(type);
        def.location = location;
        def.arraySize = size;

        m_UniformDefinitions[name] = def;
        m_UniformLocationCache[name] = location;
    }
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
