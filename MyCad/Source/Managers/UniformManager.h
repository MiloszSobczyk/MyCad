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

    template<typename Component, typename Func>
    void SetUniform(const std::string& uniformName, Func&& provider)
    {
        auto calc = GetCalculation<Component>();
        if (!calc)
        {
            calc = CreateRef<UniformCalculation>();
            m_Calculations[std::type_index(typeid(Component))] = calc;
        }
        calc->Set(uniformName, std::forward<Func>(provider));
    }

    template<typename Component>
    bool HasCalculation() const
    {
        return m_Calculations.find(std::type_index(typeid(Component))) != m_Calculations.end();
    }

    UniformValue GetUniformValue(Entity entity, const std::string& uniformName);

private:
    UniformManager() = default;
    ~UniformManager() = default;
    
    // void VerifyUniforms(); should tell if all uniforms have fallbacks

    void InitializeCalculations();
    UniformValue FallbackForUniform(const std::string& uniformName, Entity entity) const;

private:
    std::unordered_map<std::type_index, Ref<UniformCalculation>> m_Calculations;
};