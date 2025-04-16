#include "Shape.h"

int Shape::idCounter = 0;

Shape::Shape()
{
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
}

Algebra::Matrix4 Shape::GetModelMatrix() const
{
    return Algebra::Matrix4::Identity();
}


void Shape::RotateAroundPoint(Algebra::Vector4 point, Algebra::Quaternion q)
{
    //SetTranslation(point + q.Rotate(GetTranslation() - point));
    //SetRotation((GetRotation() * q.Conjugate()).Normalize());
}

void Shape::ScaleAroundPoint(Algebra::Vector4 point, Algebra::Vector4 scaleFactor)
{
    //SetTranslation(point + (GetTranslation() - point).Scale(scaleFactor));
    //SetScaling(GetScaling().Scale(scaleFactor));
}