#include "UniformManager.h"

#include <iostream>

UniformManager& UniformManager::GetInstance()
{
	static UniformManager instance;

	return instance;
}

UniformValue UniformManager::GetUniformValue(Entity entity, const std::string& uniformName)
{
    for (auto& [typeIndex, calc] : m_Calculations)
    {
        if (calc->Has(uniformName))
        {
            return calc->Get(uniformName, entity);
        }
    }

    return FallbackForUniform(uniformName, entity);
}

UniformValue UniformManager::FallbackForUniform(const std::string& uniformName, Entity entity) const
{
	if (uniformName == "u_color") 
		return DefaultColorFallback(entity); 
	if (uniformName == "u_modelMatrix") 
		return DefaultModelMatrixFallback(entity); 
	return 0;
}
