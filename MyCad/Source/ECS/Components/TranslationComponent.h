#pragma once

#include "Algebra.h"

struct TranslationComponent
{
	Algebra::Vector4 translation;

	void AddTranslation(const Algebra::Vector4& newTranslation)
	{
		translation += newTranslation;
	}
};