#pragma once

#include <string>
#include <unordered_map>
#include <functional>
#include <variant>
#include <iostream>

#include <GL/glew.h>

#include "UniformDefinition.h"
#include "Core/Scene/Entity.h"
#include "Algebra.h"

using UniformValue = std::variant<
    int,
    float,
    Algebra::Vector4,
    Algebra::Matrix4
>;

using UniformProvider = std::function<UniformValue(Entity)>;

struct UniformCalculation
{
    std::unordered_map<std::string, UniformProvider> providers;

    void Set(const std::string& name, UniformProvider func);
    bool Has(const std::string& name) const;
    UniformValue Get(const std::string& name, Entity entity) const;
};
