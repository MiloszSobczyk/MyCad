#include "UniformCalculation.h"

void UniformCalculation::Set(const std::string& name, UniformProvider func)
{
    providers[name] = std::move(func);
}

bool UniformCalculation::Has(const std::string& name) const
{
    return providers.find(name) != providers.end();
}

UniformValue UniformCalculation::Get(const std::string& name, Entity entity) const
{
    auto it = providers.find(name);
    if (it != providers.end())
    {
        return it->second(entity);
    }

    std::cout << "Warning: UniformCalculation::Get called for unknown uniform '" << name << "'\n";

    return UniformValue{ 0 };
}
