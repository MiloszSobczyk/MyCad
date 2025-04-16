#pragma once

#include "TranslationComponent.h"
#include "RotationComponent.h"
#include "ScalingComponent.h"

class ITranslation
{
public:
	virtual ~ITranslation() = default;
	virtual const TranslationComponent& GetTranslationComponent() const = 0;
};

class IRotation
{
public:
	virtual ~IRotation() = default;
	virtual const RotationComponent& GetRotationComponent() const = 0;
};

class IScaling
{
public:
	virtual ~IScaling() = default;
	virtual const ScalingComponent& GetScalingComponent() const = 0;
};