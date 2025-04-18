#include "ShaderManager.h"
#include <iostream>

ShaderManager::ShaderManager()
{
	AddShader("default", "Resources/Shaders/default");
	AddShader("defaultColor", "Resources/Shaders/defaultColor");
	AddShader("infiniteGrid", "Resources/Shaders/infiniteGrid");
	AddTessShader("bezierTess", "Resources/Shaders/bezier.vert", "Resources/Shaders/bezier.tesc", "Resources/Shaders/bezier.tese", "Resources/Shaders/bezier.frag");
}

ShaderManager& ShaderManager::GetInstance()
{
	static ShaderManager instance;

	return instance;
}

std::shared_ptr<Shader> ShaderManager::GetShader(std::string name)
{
	return shaders[name];
}

std::shared_ptr<Shader> ShaderManager::AddShader(std::string name, std::string filename)
{
	if (shaders.find(name) == shaders.end())
	{
		shaders[name] = std::make_shared<Shader>(filename);
	}

	return shaders[name];
}

std::shared_ptr<Shader> ShaderManager::AddTessShader(std::string name, std::string vsFilepath, std::string tcsFilepath, std::string tesFilepath, std::string fsFilepath)
{
	if (shaders.find(name) == shaders.end())
	{
		shaders[name] = std::make_shared<Shader>(vsFilepath, tcsFilepath, tesFilepath, fsFilepath);
	}

	return std::shared_ptr<Shader>();
}
