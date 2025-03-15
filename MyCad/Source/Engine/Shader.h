#pragma once

#include <string>
#include <unordered_map>

#include "Algebra.h"


struct ShaderProgramSource
{
	std::string VertexSource;
	std::string GeometrySource;
	std::string FragmentSource;
};

class Shader
{
private:
	std::string filepath;
	unsigned int rendererID;
	std::unordered_map<std::string, int> uniformLocationCache;

public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& name, const Algebra::Matrix4& matrix);

private:
	ShaderProgramSource ParseShader();
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& geometryShader, const std::string& fragmentShader);
	int GetUniformLocation(const std::string& name);
};
