#include <numbers>

#include "Utils.h"

float Algebra::DegreeToRadians(float degree)
{
	return std::numbers::pi_v<float> * degree / 180.f;
}

float Algebra::RadiansToDegree(float radians)
{
	return std::numbers::inv_pi_v<float> * radians * 180.f;
}
