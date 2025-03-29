#pragma once

#include "Algebra.h"
#include <imgui/imgui.h>

#pragma once

enum class RotationState
{
    Trackball = 0,
    X = 1,
    Y = 2,
    Z = 3
};


class Rotation
{
private:
    RotationState state;

public:
    inline RotationState GetState() { return state; };
    inline void SetState(RotationState state) { this->state = state; };

    Algebra::Quaternion HandleInput();
};
