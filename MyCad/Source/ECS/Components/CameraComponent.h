#pragma once

#include "Algebra.h"

struct CameraComponent {
    // Matrices
    Algebra::Matrix4 projection;
    Algebra::Matrix4 view;
    // Camera state
    Algebra::Vector4 position;
    Algebra::Vector4 direction;
    // Camera parameters
    float fov;
    float nearPlane;
    float farPlane;
};
