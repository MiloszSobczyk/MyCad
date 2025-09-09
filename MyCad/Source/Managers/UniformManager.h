#pragma once

#include "ShaderManager.h"

class UniformManager
{
public:
	static UniformManager& GetInstance();

	UniformManager(UniformManager&) = delete;
	void operator=(const UniformManager&) = delete;

private:
	UniformManager();

	void VerifyUniforms();
};