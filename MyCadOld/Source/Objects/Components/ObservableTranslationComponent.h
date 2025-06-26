#pragma once
#include "TranslationComponent.h"
#include "Core/Observable.h"

class ObservableTranslationComponent : public TranslationComponent, public Observable
{
public:
	ObservableTranslationComponent();
	ObservableTranslationComponent(const Algebra::Vector4& translation);

	void SetTranslation(const Algebra::Vector4& translation) override;
	void AddTranslation(const Algebra::Vector4& translation) override;
};