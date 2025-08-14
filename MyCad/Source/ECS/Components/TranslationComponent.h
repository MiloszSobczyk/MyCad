#pragma once

#include "Algebra.h"

struct TranslationComponent
{
	Algebra::Vector4 translation;

	TranslationComponent(Algebra::Vector4 initialTranslation = Algebra::Vector4(0.f, 0.f, 0.f, 0.f))
		: translation(initialTranslation) { }

	void AddTranslation(const Algebra::Vector4& newTranslation)
	{
		translation += newTranslation;
	}
};