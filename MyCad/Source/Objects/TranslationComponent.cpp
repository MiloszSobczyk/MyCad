#include "TranslationComponent.h"

TranslationComponent::TranslationComponent()
	: translation(0.f, 0.f, 0.f, 0.f)
{
}

TranslationComponent::TranslationComponent(const Algebra::Vector4& translation)
	: translation(translation)
{
}

Algebra::Vector4 TranslationComponent::GetTranslation() const
{
	return translation;
}

void TranslationComponent::SetTranslation(const Algebra::Vector4& translation)
{
	this->translation = translation;
}

void TranslationComponent::AddTranslation(const Algebra::Vector4& translation)
{
	this->translation += translation;
}

Algebra::Matrix4 TranslationComponent::GetTranslationMatrix() const
{
	return Algebra::Matrix4::Translation(translation);
}
