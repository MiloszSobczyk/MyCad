#pragma once

#include "Vector4.h"

namespace Algebra
{
	class Matrix4
	{
	private:
		Vector4 rows[4];
	public:
		Matrix4();
		Matrix4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3);
		Matrix4(const Vector4& diagonal);

		float Sum() const;
		Vector4 Column(int index) const;
		Matrix4 Transpose();

		Vector4& operator[](int index);
		const Vector4& operator[](int index) const;
		const Matrix4 operator+(const Matrix4& other) const;
		const Matrix4 operator-(const Matrix4& other) const;

		const Matrix4 operator*(const Matrix4& other) const;

		friend Matrix4 operator*(const Matrix4& matrix, const float& scalar);
		friend Matrix4 operator*(const float& scalar, const Matrix4& matrix);
		friend Matrix4 operator/(const Matrix4& matrix, const float& scalar);

		static Matrix4 Identity();

		static Matrix4 Translation(float x, float y, float z);

		static Matrix4 RotationX(float radians);
		static Matrix4 RotationY(float radians);
		static Matrix4 RotationZ(float radians);
		static Matrix4 Rotation(float xRadians, float yRadians, float zRadians);

		static Matrix4 RotationXByDegree(float degree);
		static Matrix4 RotationYByDegree(float degree);
		static Matrix4 RotationZByDegree(float degree);
		static Matrix4 RotationByDegree(float xDegree, float yDegree, float zDegree);
	};
}