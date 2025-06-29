#pragma once

#include "Shader.h"
#include "ShaderBuilder.h"

#include <string>
#include <unordered_map>

enum class ShaderName
{
	Default = 0,
	DefaultColor = 1,
	InfiniteGrid = 2,
	BezierCurve = 3,
	BezierSurface = 4,
	BezierSurface2 = 5,
	GregoryPatch = 6,
};

class ShaderManager 
{
public:
	static ShaderManager& GetInstance();

	ShaderManager(ShaderManager&) = delete;
	void operator=(const ShaderManager&) = delete;

	void AddShader(ShaderName name, std::shared_ptr<Shader> shader);
	inline std::shared_ptr<Shader> GetShader(ShaderName name) { return shaders[name]; }

private:
	ShaderManager();

	std::unordered_map<ShaderName, std::shared_ptr<Shader>> shaders;
};