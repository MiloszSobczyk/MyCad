#pragma once

#include "Vector4.h"

namespace Algebra
{
	struct StereoscopicMatrices;

	class Matrix4
	{
	private:
		Vector4 rows[4];

	public:
		Matrix4();
		Matrix4(const Vector4& diagonal);
		Matrix4(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4);
		Matrix4(const float& v1, const float& v2, const float& v3, const float& v4);

		friend std::ostream& operator<<(std::ostream& os, const Matrix4& matrix);

		float Sum() const;
		Matrix4 Transpose();

		Vector4 Column(int index) const;

		Vector4& operator[](int index);
		const Vector4& operator[](int index) const;
		const Matrix4 operator+(const Matrix4& other) const;
		const Matrix4 operator-(const Matrix4& other) const;

		const Matrix4 operator*(const Matrix4& matrix) const;

		friend Matrix4 operator*(const Matrix4& matrix, const float& scalar);
		friend Matrix4 operator*(const float& scalar, const Matrix4& matrix);
		friend Matrix4 operator/(const Matrix4& matrix, const float& scalar);

		static Matrix4 Identity();
		static Matrix4 Translation(float x, float y, float z);
		static Matrix4 Translation(Vector4 translation);

		// Add rotation methods for Quaternions
		static Matrix4 Rotation(float x, float y, float z);
		static Matrix4 Rotation(Vector4 rotation);
		static Matrix4 RotationX(float angle);
		static Matrix4 RotationY(float angle);
		static Matrix4 RotationZ(float angle);
		static Matrix4 RotationByDegree(float x, float y, float z);
		static Matrix4 RotationByDegree(Vector4 rotation);
		static Matrix4 RotationXByDegree(float angle);
		static Matrix4 RotationYDegree(float angle);
		static Matrix4 RotationZDegree(float angle);

		static Matrix4 Scale(float x, float y, float z);
		static Matrix4 Scale(Vector4 scale);

		static Matrix4 Projection(float aspect, float f, float n, float fov);
		static StereoscopicMatrices StereoscopicProjection(float aspect, float f, float n, float fov, float d, float c);
	};

	struct StereoscopicMatrices
	{
		Matrix4 left;
		Matrix4 right;
	};
}