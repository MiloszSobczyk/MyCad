#pragma once

#include "Engine/Shader.h"
#include <unordered_map>
#include <string>


class ShaderManager
{
private:
	std::unordered_map<std::string, std::shared_ptr<Shader>> shaders;

	ShaderManager();
	std::shared_ptr<Shader> AddShader(std::string name, std::string filename);
	std::shared_ptr<Shader> AddTessShader(std::string name, std::string vsFilepath, std::string tcsFilepath, std::string tesFilepath, std::string fsFilepath);
public:
	ShaderManager(ShaderManager& shaderManager) = delete;
	void operator=(const ShaderManager& shaderManager) = delete;
	
	static ShaderManager& GetInstance();

	std::shared_ptr<Shader> GetShader(std::string name);
};
