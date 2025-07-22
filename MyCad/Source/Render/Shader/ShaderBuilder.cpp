#include "ShaderBuilder.h"

#include "Shader.h"
#include "Utils/Logger.h"

#include <fstream>
#include <sstream>
#include <iostream>

ShaderBuilder& ShaderBuilder::SetShaderDirectory(const std::string& directory) 
{
    m_ShaderDirectory = directory;
    if (!m_ShaderDirectory.empty() && m_ShaderDirectory.back() != '/')
    {
        m_ShaderDirectory += '/';
    }

    return *this;
}

ShaderBuilder& ShaderBuilder::AddShader(ShaderType type, const std::string& name) 
{
    const ShaderTypeInfo& info = ShaderTypeInfo::Get(type);
    std::string fullPath = m_ShaderDirectory + name + info.extension;
    m_ShaderSources[type] = LoadSourceFromFile(fullPath);

    return *this;
}

std::string ShaderBuilder::LoadSourceFromFile(const std::string& filepath) 
{
    std::ifstream file(filepath);
    if (!file) 
    {
        std::cerr << "Error: cannot open shader file '" << filepath << "'\n";
        return "";
    }

    std::stringstream buffer;
    std::string line;
    while (std::getline(file, line)) 
    {
        buffer << line << '\n';
    }

    return buffer.str();
}

unsigned int ShaderBuilder::CompileShader(ShaderType type, const std::string& source) 
{
    GLenum glType = ShaderTypeInfo::Get(type).glType;

    unsigned int id = glCreateShader(glType);
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int success;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &success));
    if (!success) 
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        std::string message(length, ' ');
        GLCall(glGetShaderInfoLog(id, length, &length, &message[0]));
        std::cerr << "Error: shader compilation failed (" << (int)type << "):\n" << message << "\n";
        glDeleteShader(id);
        return 0;
    }

    return id;
}

std::shared_ptr<Shader> ShaderBuilder::Compile()
{
    unsigned int program = glCreateProgram();
    std::vector<unsigned int> shaderIDs;

    for (const auto& [type, source] : m_ShaderSources)
    {
        if (!source.empty())
        {
            unsigned int id = CompileShader(type, source);
            GLCall(glAttachShader(program, id));
            shaderIDs.push_back(id);
        }
    }

    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    for (unsigned int id : shaderIDs)
    {
        GLCall(glDeleteShader(id));
    }

    return std::make_shared<Shader>(program);
}
