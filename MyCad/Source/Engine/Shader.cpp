#include "Shader.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include "utils/GlCall.h"

Shader::Shader(const std::string& filepath)
    : m_RendererID(0)
{
    ShaderProgramSource source = GetShaderFromFiles(filepath);
    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::Shader(const std::string& vsFilepath, const std::string& fsFilepath)
{
    ShaderProgramSource source;
    std::string vertexShaderSource = ParseShader(vsFilepath).str();
    std::string fragmentShaderSource = ParseShader(fsFilepath).str();

    if (vertexShaderSource.empty())
    {
        std::cerr << "WARNING: Vertex shader (" << vsFilepath << ") is empty\n";
    }

    if (fragmentShaderSource.empty())
    {
        std::cerr << "WARNING: Fragment shader (" << fsFilepath << ") is empty\n";
    }

    m_RendererID = CreateShader(source.VertexSource, source.FragmentSource);
}

Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

void Shader::SetUniformMat4f(const std::string& name, const Algebra::Matrix4& matrix)
{
    GLCall(glUniformMatrix4fv(GetUniformLocation(name), 1, GL_TRUE, &matrix[0][0]));
}

void Shader::SetUniformVec4f(const std::string& name, const Algebra::Vector4& vector)
{
    GLCall(glUniform4f(GetUniformLocation(name), vector.x, vector.y, vector.z, vector.w));
}

std::stringstream Shader::ParseShader(const std::string& filepath)
{
    std::ifstream stream(filepath);

    std::string line;
    std::stringstream ss;
    while (getline(stream, line))
    {
        ss << line << '\n';
    }

    return ss;
}

ShaderProgramSource Shader::GetShaderFromFiles(const std::string& filepath)
{
    std::stringstream vertexShaderSource = ParseShader(filepath + ".vert");
    std::stringstream fragmentShaderSource = ParseShader(filepath + ".frag");

    if (vertexShaderSource.str().empty())
    {
        std::cerr << "WARNING: Vertex shader (" << filepath + ".vert" << ") is empty\n";
    }

    if (fragmentShaderSource.str().empty())
    {
        std::cerr << "WARNING: Fragment shader (" << filepath + ".frag" << ") is empty\n";
    }

    return { vertexShaderSource.str(), fragmentShaderSource.str() };
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    GLCall(glShaderSource(id, 1, &src, nullptr));
    GLCall(glCompileShader(id));

    int result;
    GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
    if (result == GL_FALSE)
    {
        int length;
        GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
        char* message = (char*)alloca(length * sizeof(char));
        GLCall(glGetShaderInfoLog(id, length, &length, message));
        printf("Failed to compile %s shader!\n", (type == GL_VERTEX_SHADER) ? "vertex" : "fragment");
        std::cout << message << std::endl;
        GLCall(glDeleteShader(id));
        return 0;
    }

    return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    GLCall(glAttachShader(program, vs));
    GLCall(glAttachShader(program, fs));
    GLCall(glLinkProgram(program));
    GLCall(glValidateProgram(program));

    GLCall(glDeleteShader(vs));
    GLCall(glDeleteShader(fs));

    return program;
}

void Shader::Bind() const
{
    GLCall(glUseProgram(m_RendererID));
}

void Shader::UnBind() const
{
    GLCall(glUseProgram(0));
}

int Shader::GetUniformLocation(const std::string& name)
{
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLCall(unsigned int location = glGetUniformLocation(m_RendererID, name.c_str()));
    if (location == -1) printf("Warning: uniform '%s' doesn't exist!\n", name.c_str());
    else m_UniformLocationCache[name] = location;

    return location;
}