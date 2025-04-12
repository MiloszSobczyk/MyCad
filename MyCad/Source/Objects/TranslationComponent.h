#pragma once

#include "Algebra.h"

class TranslationComponent
{
private:
    Algebra::Vector4 translation;

public:
    TranslationComponent();
    TranslationComponent(const Algebra::Vector4& translation);

    Algebra::Vector4 GetTranslation() const;
    void SetTranslation(const Algebra::Vector4& translation);
    void AddTranslation(const Algebra::Vector4& translation);
    Algebra::Matrix4 GetTranslationMatrix() const;
};