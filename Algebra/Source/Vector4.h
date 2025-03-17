#pragma once
#include <cstddef>
#include <vector>
#include <ostream>

namespace Algebra
{
	class Matrix4;

	class Vector4
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Vector4();
		Vector4(float x, float y, float z, float w = 0.f);

		float Length() const;
		float Sum() const;
		const Vector4 Scale(const Vector4& scale) const;
		Vector4 Normalize() const;
		std::vector<float> ToVector() const;

		float& operator[](int index);
		const float& operator[](int index) const;
		const Vector4 operator+(const Vector4& other) const;
		const Vector4 operator-(const Vector4& other) const;
		const Vector4 operator-() const;

		const float operator*(const Vector4& other) const;
		const bool operator==(const Vector4& other) const;

		Vector4& operator+=(const Vector4& add);

		friend Vector4 operator*(const Vector4& vector, const Matrix4& matrix);
		friend Vector4 operator*(const Matrix4& matrix, const Vector4& vector);
		friend Vector4 operator*(const Vector4& vector, const float& scalar);
		friend Vector4 operator*(const float& scalar, const Vector4& vector);
		friend Vector4 operator/(const Vector4& vector, const float& scalar);

		friend std::ostream& operator<<(std::ostream& os, const Vector4& vector);

		static Vector4 Cross(Vector4 v1, Vector4 v2);
	};
}