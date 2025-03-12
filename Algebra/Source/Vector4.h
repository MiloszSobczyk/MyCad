#pragma once
#include <cstddef>

namespace Algebra
{
	class Matrix4;

	class Vector4
	{
	public:
		float x, y, z, w;

		Vector4();
		Vector4(float x, float y, float z, float w);

		float Sum() const;
		float Length() const;
		Vector4 Normalize() const;
		Vector4 Cross(const Vector4& other) const;

		float& operator[](int index);
		const float& operator[](int index) const;
		const Vector4 operator+(const Vector4& other) const;
		const Vector4 operator-(const Vector4& other) const;
		const Vector4 operator-() const;

		const float operator*(const Vector4& other) const;
		const Vector4 operator*(const Matrix4& matrix) const;

		friend Vector4 operator*(const Vector4& vector, const float& scalar);
		friend Vector4 operator*(const float& scalar, const Vector4& vector);
		friend Vector4 operator/(const Vector4& vector, const float& scalar);
	};
}

