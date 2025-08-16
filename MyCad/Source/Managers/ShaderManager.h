#pragma once

#include "Render/Shader/Shader.h"
#include "Render/Shader/ShaderBuilder.h"

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

	void AddShader(ShaderName name, Ref<Shader> shader);
	inline Ref<Shader> GetShader(ShaderName name) { return m_Shaders[name]; }

private:
	ShaderManager();

	std::unordered_map<ShaderName, Ref<Shader>> m_Shaders;
};