#include "ShaderManager.h"
#include <iostream>

ShaderManager::ShaderManager()
{
	AddShader(
		ShaderName::Default,
		ShaderBuilder()
		.SetShaderDirectory("Resources/Shaders/")
		.AddShader(ShaderType::Vertex, "default")
		.AddShader(ShaderType::Fragment, "default")
		.Compile()
	);

	AddShader(
		ShaderName::DefaultColor,
		ShaderBuilder()
		.SetShaderDirectory("Resources/Shaders/")
		.AddShader(ShaderType::Vertex, "defaultColor")
		.AddShader(ShaderType::Fragment, "defaultColor")
		.Compile()
	);

	AddShader(
		ShaderName::InfiniteGrid,
		ShaderBuilder()
		.SetShaderDirectory("Resources/Shaders/")
		.AddShader(ShaderType::Vertex, "infiniteGrid")
		.AddShader(ShaderType::Fragment, "infiniteGrid")
		.Compile()
	);

	AddShader(
		ShaderName::BezierCurve,
		ShaderBuilder()
		.SetShaderDirectory("Resources/Shaders/")
		.AddShader(ShaderType::Vertex, "bezierCurve")
		.AddShader(ShaderType::TessControl, "bezierCurve")
		.AddShader(ShaderType::TessEvaluation, "bezierCurve")
		.AddShader(ShaderType::Fragment, "bezierCurve")
		.Compile()
	);

	AddShader(
		ShaderName::BezierSurface,
		ShaderBuilder()
		.SetShaderDirectory("Resources/Shaders/")
		.AddShader(ShaderType::Vertex, "bezierSurface")
		.AddShader(ShaderType::TessControl, "bezierSurface")
		.AddShader(ShaderType::TessEvaluation, "bezierSurface")
		.AddShader(ShaderType::Fragment, "bezierSurface")
		.Compile()
	);

	AddShader(
		ShaderName::BezierSurface2,
		ShaderBuilder()
		.SetShaderDirectory("Resources/Shaders/")
		.AddShader(ShaderType::Vertex, "bezierSurface")
		.AddShader(ShaderType::TessControl, "bezierSurface2")
		.AddShader(ShaderType::TessEvaluation, "bezierSurface2")
		.AddShader(ShaderType::Fragment, "bezierSurface")
		.Compile()
	);
}

ShaderManager& ShaderManager::GetInstance()
{
	static ShaderManager instance;

	return instance;
}

void ShaderManager::AddShader(ShaderName name, std::shared_ptr<Shader> shader)
{
	if (shaders.find(name) != shaders.end())
	{
		std::cerr << "Shader already exists: " << static_cast<int>(name) << '\n';
		return;
	}

	shaders[name] = shader;
}

std::shared_ptr<Shader> ShaderManager::GetShader(ShaderName name)
{
	return shaders[name];
}
