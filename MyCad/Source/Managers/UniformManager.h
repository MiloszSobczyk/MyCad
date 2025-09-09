#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>
#include <iostream>

#include "Render/Uniform/UniformCalculation.h"
#include "Render/Uniform/UniformCalculationFallbacks.h"
#include "Core/Scene/Entity.h"

class UniformManager
{
public:
    static UniformManager& GetInstance();

    UniformManager(UniformManager&) = delete;
    void operator=(const UniformManager&) = delete;

    template<typename Component>
    void RegisterCalculation(UniformCalculation calc)
    {
        m_Calculations[std::type_index(typeid(Component))] = std::make_shared<UniformCalculation>(std::move(calc));
    }

    template<typename Component, typename Func>
    void SetUniform(const std::string& uniformName, Func&& provider)
    {
        auto calc = GetCalculation<Component>();
        if (!calc)
        {
            calc = std::make_shared<UniformCalculation>();
            m_Calculations[std::type_index(typeid(Component))] = calc;
        }
        calc->Set(uniformName, std::forward<Func>(provider));
    }

    template<typename Component>
    std::shared_ptr<UniformCalculation> GetCalculation() const
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

    UniformValue GetUniformValue(Entity entity, const std::string& uniformName);

private:
    UniformValue FallbackForUniform(const std::string& uniformName, Entity entity) const;

private:
    UniformManager() = default;
    ~UniformManager() = default;

    std::unordered_map<std::type_index, std::shared_ptr<UniformCalculation>> m_Calculations;
};