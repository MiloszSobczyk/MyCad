#pragma once

#include <string>

#include <GL/glew.h>

enum class UniformType
{
    Float,
    Vec2,
    Vec3,
    Vec4,
    Mat3,
    Mat4,
    Int,
    Sampler2D,
    SamplerCube,
    Unknown
};

inline UniformType ToUniformType(GLenum glType)
{
    switch (glType)
    {
    case GL_FLOAT:          return UniformType::Float;
    case GL_FLOAT_VEC2:     return UniformType::Vec2;
    case GL_FLOAT_VEC3:     return UniformType::Vec3;
    case GL_FLOAT_VEC4:     return UniformType::Vec4;
    case GL_FLOAT_MAT3:     return UniformType::Mat3;
    case GL_FLOAT_MAT4:     return UniformType::Mat4;
    case GL_INT:            return UniformType::Int;
    case GL_SAMPLER_2D:     return UniformType::Sampler2D;
    case GL_SAMPLER_CUBE:   return UniformType::SamplerCube;
    default:                return UniformType::Unknown;
    }
}

struct UniformDefinition
{
    std::string name;
    UniformType type;
    int location;
    int arraySize;
};