#pragma once

#include "Engine/Shader.h"
#include "Engine/ShaderBuilder.h"
#include <string>
#include <unordered_map>

enum class ShaderName
{
	Default = 0,
	DefaultColor = 1,
	InfiniteGrid = 2,
	BezierCurve = 3,
};

class ShaderManager
{
private:
	std::unordered_map<ShaderName, std::shared_ptr<Shader>> shaders;

	ShaderManager();
public:
	ShaderManager(ShaderManager& shaderManager) = delete;
	void operator=(const ShaderManager& shaderManager) = delete;
	
	static ShaderManager& GetInstance();

	void AddShader(ShaderName name, std::shared_ptr<Shader> shader);
	std::shared_ptr<Shader> GetShader(ShaderName name);
};
