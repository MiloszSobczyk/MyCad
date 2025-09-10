#pragma once

#include "Render/Uniform/UniformCalculation.h"
#include "Render/Uniform/UniformCalculationFallbacks.h"
#include "Core/Scene/Entity.h"
#include "Utils/Base.h"

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <iostream>

class UniformManager
{
public:
    static UniformManager& GetInstance();

    UniformManager(UniformManager&) = delete;
    void operator=(const UniformManager&) = delete;

    template<typename Component>
    void RegisterCalculation(UniformCalculation calc)
    {
        m_Calculations[std::type_index(typeid(Component))] = CreateRef<UniformCalculation>(std::move(calc));
    }

    template<typename Component>
    Ref<UniformCalculation> GetCalculation() const
    {
        auto it = m_Calculations.find(std::type_index(typeid(Component)));
        if (it != m_Calculations.end())
            return it->second;
        return nullptr;
    }

    template<typename Component>
    bool HasCalculation() const
    {
        return m_Calculations.find(std::type_index(typeid(Component))) != m_Calculations.end();
    }

private:
    UniformManager();
    ~UniformManager() = default;
    
    // void VerifyUniforms(); should tell if all uniforms have fallbacks

    void InitializeCalculations();
    UniformProvider FallbackForUniform(const std::string& uniformName) const;

private:
    std::unordered_map<std::type_index, Ref<UniformCalculation>> m_Calculations;
};