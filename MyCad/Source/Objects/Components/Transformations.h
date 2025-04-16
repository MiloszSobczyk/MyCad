#pragma once

#include "TranslationComponent.h"
#include "RotationComponent.h"
#include "ScalingComponent.h"

class ITranslation
{
public:
	virtual ~ITranslation() = default;
	virtual TranslationComponent& GetTranslationComponent() = 0;
};

class IRotation
{
public:
	virtual ~IRotation() = default;
	virtual RotationComponent& GetRotationComponent() = 0;
};

class IScaling
{
public:
	virtual ~IScaling() = default;
	virtual ScalingComponent& GetScalingComponent() = 0;
};