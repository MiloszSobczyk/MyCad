#include "Shader.h"

#include "Utils/Logger.h"

#include <GL/glew.h>

#include <iostream>

Shader::Shader(unsigned int programID)
    : programID(programID) 
{
}

Shader::~Shader() 
{
    glDeleteProgram(programID);
}

void Shader::Bind() const 
{
    GLCall(glUseProgram(programID));
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
    if (uniformLocationCache.find(name) != uniformLocationCache.end())
    {
        return uniformLocationCache[name];
    }

    GLCall(unsigned int location = glGetUniformLocation(programID, name.c_str()));
    if (location == -1)
    {
		std::cerr << "Warning: uniform '" << name << "' doesn't exist in shader program!\n";
    }

    uniformLocationCache[name] = location;
    return location;
}
