#include "UniformManager.h"

#include <iostream>

UniformManager& UniformManager::GetInstance()
{
	static UniformManager instance;

	return instance;
}

UniformManager::UniformManager()
{
    InitializeCalculations();
}

void UniformManager::InitializeCalculations()
{
    {
        UniformCalculation pointCalculation;
        pointCalculation.Set("u_color", FallbackForUniform("u_color"));
        pointCalculation.Set("u_modelMatrix", FallbackForUniform("u_modelMatrix"));
        RegisterCalculation<PointComponent>(pointCalculation);
    }
    {
		UniformCalculation torusCalculation;
        torusCalculation.Set("u_color", FallbackForUniform("u_color"));
        torusCalculation.Set("u_modelMatrix", FallbackForUniform("u_modelMatrix"));
		RegisterCalculation<TorusComponent>(torusCalculation);
    }
    {
        UniformCalculation polylineCalculation;
        polylineCalculation.Set("u_color", FallbackForUniform("u_color"));
        polylineCalculation.Set("u_modelMatrix", FallbackForUniform("u_modelMatrix"));
		RegisterCalculation<PolylineComponent>(polylineCalculation);
    }
}

UniformProvider UniformManager::FallbackForUniform(const std::string& uniformName) const
{
    static const std::unordered_map<std::string, UniformProvider> fallbackMap
    {
        { "u_color", DefaultColorFallback },
        { "u_modelMatrix", DefaultModelMatrixFallback }
    };

    auto it = fallbackMap.find(uniformName);
    if (it != fallbackMap.end())
        return it->second;

    return [](Entity) -> UniformValue { return 0; };
}
