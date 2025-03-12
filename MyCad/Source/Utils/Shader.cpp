#include "Shader.h"
#include "Logger.h"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader(const std::string& filepath) 
	: filepath(filepath)
{
	ShaderProgramSource source = ParseShader(filepath);
	rendererID = CreateShader(source.VertexSource, source.GeometrySource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(rendererID);
}

void Shader::Bind() const
{
	GLCall(glUseProgram(rendererID));
}

void Shader::Unbind() const
{
	GLCall(glUseProgram(0));
}

void Shader::SetUniform1i(const std::string& name, int value)
{
	GLCall(glUniform1i(GetUniformLocation(name), value));
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLCall(glUniform4f(GetUniformLocation(name), v0, v1, v2, v3));
}

ShaderProgramSource Shader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	enum class ShaderType
	{
		NONE = -1, VERTEX = 0, GEOMETRY = 1, FRAGMENT = 2
	};

	std::string line;
	std::stringstream ss[3];
	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
				type = ShaderType::VERTEX;
			else if (line.find("geometry") != std::string::npos)
				type = ShaderType::GEOMETRY;
			else if (line.find("fragment") != std::string::npos)
				type = ShaderType::FRAGMENT;
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}

	return { ss[0].str(), ss[1].str(), ss[2].str() };
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

		std::string shaderType;
		switch (type)
		{
		case GL_VERTEX_SHADER:
			shaderType = "vertex";
			break;
		case GL_GEOMETRY_SHADER:
			shaderType = "geometry";
			break;
		case GL_FRAGMENT_SHADER:
			shaderType = "fragment";
			break;
		default:
			shaderType = "unknown";
			break;
		}

		std::cerr << "Failed to compile" << shaderType << " shader!\n";
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

unsigned int Shader::CreateShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = 0, gs = 0, fs = 0;

	if (!vertexShader.empty())
	{
		vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
		GLCall(glAttachShader(program, vs));
	}
	else
	{
		std::cerr << "Warning: Vertex shader is empty!\n";
	}

	if (!geometryShader.empty())
	{
		gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
		GLCall(glAttachShader(program, gs));
	}
	else
	{
		std::cerr << "Warning: Geometry shader is empty (this may be intentional)!\n";
	}

	if (!fragmentShader.empty())
	{
		fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);
		GLCall(glAttachShader(program, fs));
	}
	else
	{
		std::cerr << "Warning: Fragment shader is empty!\n";
	}

	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	if (vs) GLCall(glDeleteShader(vs));
	if (gs) GLCall(glDeleteShader(gs));
	if (fs) GLCall(glDeleteShader(fs));

	return program;
}


int Shader::GetUniformLocation(const std::string& name)
{
	if (uniformLocationCache.find(name) != uniformLocationCache.end())
	{
		return uniformLocationCache[name];
	}

	GLCall(unsigned int location = glGetUniformLocation(rendererID, name.c_str()));
	if (location == -1)
	{
		std::cout << "Warning: uniform " << name << " doesn't exist!\n";
	}
	else
	{
		uniformLocationCache[name] = location;
	}

	return location;
}
