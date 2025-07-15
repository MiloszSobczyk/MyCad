#pragma once

namespace Algebra
{
	class Matrix4;
	class Vector4;
	class Quaternion;
}

class CameraLogic
{
public:
	Algebra::Matrix4 GetViewMatrix(const Algebra::Vector4& translation, const Algebra::Quaternion& rotation) const;

private:
	Algebra::Vector4 GetUpVector(const Algebra::Quaternion& rotation) const;
	Algebra::Vector4 GetRightVector(const Algebra::Quaternion& rotation) const;
};