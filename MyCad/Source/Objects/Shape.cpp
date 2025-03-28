#include "Shape.h"

int Shape::idCounter = 0;

Shape::Shape() 
	: id(++idCounter), color(0.5f, 0.f, 0.5f, 1.0f), scaling(1.f, 1.f, 1.f)
{
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
	auto t = GetTranslationMatrix();
	auto r = GetRotationMatrix();
	auto s = GetScaleMatrix();

    return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
}

void Shape::RenderUI()
{
    if (ImGui::Begin("Shape Properties"))
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

        ImGui::InputFloat("Scale", &scaling.x);
        if (ImGui::IsItemDeactivatedAfterEdit())
        {
            scaling = Algebra::Vector4(scaling.x, scaling.y, scaling.z, 0);
        }

        ImGui::End();
    }
}