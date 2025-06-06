#include "Shape.h"

int Shape::idCounter = 0;

void Shape::Update()
{
}

Shape::Shape()
    : id(idCounter++), name("Shape" + std::to_string(id))
{
    color = ColorPalette::Get(Color::Purple);
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