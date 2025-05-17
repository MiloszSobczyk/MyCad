#include "ColorPalette.h"

std::unordered_map<Color, Algebra::Vector4> ColorPalette::colors = {
    { Color::Red, Algebra::Vector4(1.f, 0.f, 0.f, 1.f) },
    { Color::Green, Algebra::Vector4(0.f, 1.f, 0.f, 1.f) },
    { Color::Blue, Algebra::Vector4(0.f, 0.f, 1.f, 1.f) },
    { Color::Teal, Algebra::Vector4(0.f, 0.8f, 0.8f, 1.f) },
    { Color::Purple, Algebra::Vector4(0.5f, 0.1f, 0.5f, 1.f) },
    { Color::Emerald, Algebra::Vector4(0.f, 0.8f, 0.f, 1.f) },
};

const Algebra::Vector4& ColorPalette::Get(Color color)
{
    return colors.at(color);
}