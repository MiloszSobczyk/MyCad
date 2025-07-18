#pragma once

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

private:
    unsigned int programID;
    mutable std::unordered_map<std::string, int> uniformLocationCache;

    int GetUniformLocation(const std::string& name);
};
