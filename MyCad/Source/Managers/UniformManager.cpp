#include "UniformManager.h"

#include <iostream>

UniformManager::UniformManager()
{
	VerifyUniforms();
}

UniformManager& UniformManager::GetInstance()
{
	static UniformManager instance;

	return instance;
}

void UniformManager::VerifyUniforms()
{
	std::cout << "Verifying uniforms...\n";
}
