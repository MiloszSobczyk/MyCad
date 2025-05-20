#pragma once

#include "Algebra.h"

#include <unordered_map>

enum class Color
{
    Red,
    Green,
    Blue,
    Purple,
    Teal,
    Emerald,
};

class ColorPalette
{
private:
    ColorPalette() = delete;
    ~ColorPalette() = delete;
    ColorPalette(const ColorPalette&) = delete;
    ColorPalette& operator=(const ColorPalette&) = delete;

    static std::unordered_map<Color, Algebra::Vector4> colors;

public:
    static const Algebra::Vector4& Get(Color color);
};
