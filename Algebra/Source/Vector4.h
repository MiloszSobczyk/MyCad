#pragma once

#include <cstddef>
#include <iostream>

namespace Algebra
{
	class Matrix4;

	class Vector4
	{
	public:
		float x, y, z, w;

		Vector4();
		Vector4(float x, float y, float z, float w = 0.f);

		friend std::ostream& operator<<(std::ostream& os, const Vector4& vector);

		float Sum() const;
		float Length() const;
		Vector4 Normalize() const;

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
		
		static Vector4 Cross(const Vector4& a, const Vector4& b);
	};
}

