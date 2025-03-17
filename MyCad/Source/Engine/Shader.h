#pragma once

#include <string>
#include <unordered_map>
#include "Algebra.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	Shader(const std::string& vsFilepath, const std::string& fsFilepath);
	~Shader();

	void SetUniformMat4f(const std::string& name, const Algebra::Matrix4& matrix);
	void SetUniformVec4f(const std::string& name, const Algebra::Vector4& vector);

	void Bind() const;
	void UnBind() const;
private:
	std::stringstream ParseShader(const std::string& filepath);
	ShaderProgramSource GetShaderFromFiles(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name);
};
