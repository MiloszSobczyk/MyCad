#include "Globals.h"
#include <numbers>

const int Globals::StartingWidth = 1280;
const int Globals::StartingHeight = 960;
const int Globals::RightInterfaceWidth = 350;
const float Globals::NearPlane = 0.1f;
const float Globals::FarPlane = 1000.f;
const float Globals::Fov = std::numbers::pi_v<float> / 2.f;
const Algebra::Vector4 Globals::StartingCameraPosition = Algebra::Vector4(0.f, 0.f, 10.f, 1.f);