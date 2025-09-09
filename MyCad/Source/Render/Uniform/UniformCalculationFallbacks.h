#pragma once

#include "UniformCalculation.h"

#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

#include "Algebra.h"

inline UniformValue DefaultColorFallback(Entity e)
{
    return Algebra::Vector4{ 1.0f, 1.0f, 1.0f, 1.0f };
}

inline UniformValue DefaultModelMatrixFallback(Entity e)
{
    auto modelMatrix = Algebra::Matrix4::Identity();

    if (e.HasComponent<ScalingComponent>())
    {
        const auto& scaling = e.GetComponent<ScalingComponent>();
        modelMatrix = Algebra::Matrix4::Scale(scaling.scaling) * modelMatrix;
    }

    if (e.HasComponent<RotationComponent>())
    {
        const auto& rotation = e.GetComponent<RotationComponent>();
        modelMatrix = Algebra::Matrix4::Rotation(rotation.rotation) * modelMatrix;
    }

    if (e.HasComponent<TranslationComponent>())
    {
        const auto& translation = e.GetComponent<TranslationComponent>();
        modelMatrix = Algebra::Matrix4::Translation(translation.translation) * modelMatrix;
    }

    return modelMatrix;
}