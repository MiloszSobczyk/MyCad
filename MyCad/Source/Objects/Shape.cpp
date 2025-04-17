#include "Shape.h"

int Shape::idCounter = 0;

Shape::Shape()
    : id(idCounter++), name("Shape" + std::to_string(id)), color(0.5f, 0.1f, 0.5f, 1.0f)
{
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