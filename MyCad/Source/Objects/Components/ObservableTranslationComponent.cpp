#include "ObservableTranslationComponent.h"

ObservableTranslationComponent::ObservableTranslationComponent()
	: TranslationComponent()
{
}

ObservableTranslationComponent::ObservableTranslationComponent(const Algebra::Vector4& translation)
	: TranslationComponent(translation)
{
}

void ObservableTranslationComponent::SetTranslation(const Algebra::Vector4& translation)
{
	TranslationComponent::SetTranslation(translation);
	NotifyObservers();
}

void ObservableTranslationComponent::AddTranslation(const Algebra::Vector4& translation)
{
	TranslationComponent::AddTranslation(translation);
	NotifyObservers();
}
