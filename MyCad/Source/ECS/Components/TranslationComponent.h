#pragma once

#include "Core/Observable.h"

#include "Algebra.h"

struct TranslationComponent
{
	Observable<Algebra::Vector4> translation;

	TranslationComponent() = default;
	TranslationComponent(const TranslationComponent&) = default;

	void AddTranslation(const Algebra::Vector4& newTranslation)
	{
		translation.Set(translation.value + newTranslation);
	}

	void SetTranslation(const Algebra::Vector4& newTranslation)
	{
		translation.Set(newTranslation);
	}
};