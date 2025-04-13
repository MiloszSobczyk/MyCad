#include "Shape.h"

int Shape::idCounter = 0;

Shape::Shape() 
	: id(++idCounter), color(0.5f, 0.f, 0.5f, 1.0f)
{
    //TODO: Consider if Shape should have the components allocated or they should be nullptrs
    scalingComponent = std::make_unique<ScalingComponent>();
    rotationComponent = std::make_unique<RotationComponent>();
    translationComponent = std::make_unique<TranslationComponent>();

    name = "Shape" + std::to_string(GetId());
}

void Shape::RenderUI()
{
    char nameBuffer[128];
    strncpy_s(nameBuffer, name.c_str(), sizeof(nameBuffer));
    nameBuffer[sizeof(nameBuffer) - 1] = '\0';
    if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
    {
        name = std::string(nameBuffer);
    }

    float colorArray[4] = { color.x, color.y, color.z, color.w };
    if (ImGui::ColorEdit4("Color", colorArray))
    {
        color = Algebra::Vector4(colorArray[0], colorArray[1], colorArray[2], colorArray[3]);
    }

    if (translationComponent)
        translationComponent->RenderUI();

    if (scalingComponent)
        scalingComponent->RenderUI();
}

void Shape::RotateAroundPoint(Algebra::Vector4 point, Algebra::Quaternion q)
{
    SetTranslation(point + q.Rotate(GetTranslation() - point));
    SetRotation((GetRotation() * q.Conjugate()).Normalize());
}

void Shape::ScaleAroundPoint(Algebra::Vector4 point, Algebra::Vector4 scaleFactor)
{
    SetTranslation(point + (GetTranslation() - point).Scale(scaleFactor));
    SetScaling(GetScaling().Scale(scaleFactor));
}

Algebra::Matrix4 Shape::GetModelMatrix() const
{
    return GetTranslationMatrix() * GetRotationMatrix() * GetScalingMatrix();
}

void Shape::SetScaling(const Algebra::Vector4& scaling) const
{
    if (scalingComponent)
        scalingComponent->SetScaling(scaling);
}

void Shape::AddScaling(const Algebra::Vector4& scaling) const
{
    if (scalingComponent)
        scalingComponent->AddScaling(scaling);
}

Algebra::Vector4 Shape::GetScaling() const
{
    return scalingComponent ? scalingComponent->GetScaling() : Algebra::Vector4(1.f, 1.f, 1.f);
}

Algebra::Matrix4 Shape::GetScalingMatrix() const
{
    return scalingComponent ? scalingComponent->GetScalingMatrix() : Algebra::Matrix4::Identity();
}

void Shape::SetRotation(const Algebra::Quaternion& rotation) const
{
    if (rotationComponent)
        rotationComponent->SetRotation(rotation);
}

void Shape::AddRotation(const Algebra::Quaternion& rotation) const
{
    if (rotationComponent)
        rotationComponent->AddRotation(rotation);
}

Algebra::Quaternion Shape::GetRotation() const
{
    return rotationComponent ? rotationComponent->GetRotation() : Algebra::Quaternion(1.f, 0.f, 0.f, 0.f);
}

Algebra::Matrix4 Shape::GetRotationMatrix() const
{
    return rotationComponent ? rotationComponent->GetRotationMatrix() : Algebra::Matrix4::Identity();
}

void Shape::SetTranslation(const Algebra::Vector4& translation) const
{
    if (translationComponent)
        translationComponent->SetTranslation(translation);
}

void Shape::AddTranslation(const Algebra::Vector4& translation) const
{
    if (translationComponent)
        translationComponent->AddTranslation(translation);
}

Algebra::Vector4 Shape::GetTranslation() const
{
    return translationComponent ? translationComponent->GetTranslation() : Algebra::Vector4();
}

Algebra::Matrix4 Shape::GetTranslationMatrix() const
{
    return translationComponent ? translationComponent->GetTranslationMatrix() : Algebra::Matrix4::Identity();
}
