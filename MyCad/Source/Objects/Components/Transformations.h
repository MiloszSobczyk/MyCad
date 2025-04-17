#pragma once

#include <memory>

#include "TranslationComponent.h"
#include "RotationComponent.h"
#include "ScalingComponent.h"

class ITranslation
{
public:
	virtual ~ITranslation() = default;
	virtual std::shared_ptr<TranslationComponent> GetTranslationComponent() = 0;
};

class IRotation
{
public:
	virtual ~IRotation() = default;
	virtual std::shared_ptr<RotationComponent> GetRotationComponent() = 0;
};

class IScaling
{
public:
	virtual ~IScaling() = default;
	virtual std::shared_ptr<ScalingComponent> GetScalingComponent() = 0;
};