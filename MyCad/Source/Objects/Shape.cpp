#include "Shape.h"

int Shape::idCounter = 0;

Shape::Shape() 
	: id(++idCounter), color(0.5f, 0.f, 0.5f, 1.0f), scaling(1.f, 1.f, 1.f)
{
    name = "Shape" + std::to_string(GetId());
}

Algebra::Matrix4 Shape::GetRotationMatrix()
{
    return rotation.ToMatrix();
}

Algebra::Matrix4 Shape::GetTranslationMatrix()
{
    return Algebra::Matrix4::Translation(translation);
}

Algebra::Matrix4 Shape::GetScaleMatrix()
{
    return Algebra::Matrix4::Scale(scaling);
}

Algebra::Matrix4 Shape::GetModelMatrix()
{
    return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
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

    ImGui::InputFloat3("Translation", &translation.x);
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        translation = Algebra::Vector4(translation.x, translation.y, translation.z, 0);
    }

    static float prevScaling = scaling.x;
    float tempScaling = scaling.x;
    if (ImGui::InputFloat("Scaling", &tempScaling))
    {
        if (tempScaling > 0.1f)
        {
            prevScaling = tempScaling;
        }
        else
        {
            tempScaling = prevScaling;
        }
    }
    if (ImGui::IsItemDeactivatedAfterEdit())
    {
        scaling = Algebra::Vector4(tempScaling, tempScaling, tempScaling, 0);
    }
}

void Shape::RotateAroundPoint(Algebra::Vector4 point, Algebra::Quaternion q)
{
    // Rotate the vector (translation - point) using the quaternion's Rotate method.
    translation = point + q.Rotate(translation - point);
    // Update the shape's rotation by composing the new rotation with the current one.
    rotation = q * rotation;
}
