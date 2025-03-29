#pragma once

#include "Algebra.h"
#include <imgui/imgui.h>

enum class TranslationState
{
	Camera = 0,
	X = 1,
	Y = 2,
	Z = 3
};

class Translation
{
private:
	TranslationState state;
public:
	inline TranslationState GetState() { return state; };
	inline void SetState(TranslationState state) { this->state = state; };

	Algebra::Vector4 HandleInput();
};