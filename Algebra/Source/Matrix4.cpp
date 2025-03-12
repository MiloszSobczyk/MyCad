#include "Matrix4.h"

#include <stdexcept>
#include <numbers>
#include <iostream>

using namespace Algebra;

float ConvertDegreeToRadians(float degree)
{
	return std::numbers::pi_v<float> * degree / 180.f;
}

Matrix4::Matrix4()
{
	for (int i = 0; i < 4; ++i)
	{
		rows[i] = Vector4();
	}
}

Matrix4::Matrix4(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3)
{
	rows[0] = row0;
	rows[1] = row1;
	rows[2] = row2;
	rows[3] = row3;
}

Matrix4::Matrix4(const Vector4& diagonal)
	: Matrix4()
{
	for (int i = 0; i < 4; ++i)
	{
		rows[i][i] = diagonal[i];
	}
}

float Matrix4::Sum() const
{
	float result = 0.f;
	for (int i = 0; i < 4; ++i)
	{
		result += rows[i].Sum();
	}
	return result;
}

Vector4 Matrix4::Column(int index) const
{
	return Vector4(rows[0][index], rows[1][index], rows[2][index], rows[3][index]);
}

Matrix4 Matrix4::Transpose()
{
	return Matrix4(this->Column(0), this->Column(1), this->Column(2), this->Column(3));
}

Vector4& Matrix4::operator[](int index)
{
	return rows[index];
}

const Vector4& Matrix4::operator[](int index) const
{
	return rows[index];
}

const Matrix4 Matrix4::operator+(const Matrix4& other) const
{
	return Matrix4(
		this->rows[0] + other[0], 
		this->rows[1] + other[1],
		this->rows[2] + other[2], 
		this->rows[3] + other[3]
	);
}

const Matrix4 Matrix4::operator-(const Matrix4& other) const
{
	return Matrix4(
		this->rows[0] - other[0], 
		this->rows[1] - other[1],
		this->rows[2] - other[2], 
		this->rows[3] - other[3]
	);
}

const Matrix4 Matrix4::operator*(const Matrix4& other) const
{
	Matrix4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = (*this)[i] * other.Column(j);
		}
	}

	return result;
}

Matrix4 Algebra::operator*(const Matrix4& matrix, const float& scalar)
{
	return Matrix4(matrix[0] * scalar, matrix[1] * scalar, matrix[2] * scalar, matrix[3] * scalar);
}

Matrix4 Algebra::operator*(const float& scalar, const Matrix4& matrix)
{
	return matrix * scalar;
}

Matrix4 Algebra::operator/(const Matrix4& matrix, const float& scalar)
{
	if (scalar == 0)
	{
		throw std::runtime_error("Scalar cannot be zero");
	}

	return matrix * (1.f / scalar);
}

Matrix4 Matrix4::Identity()
{
	return Matrix4(Vector4(1, 1, 1, 1));
}

Matrix4 Matrix4::Translation(float x, float y, float z)
{
	Matrix4 result = Matrix4::Identity();
	result[0][3] = x;
	result[1][3] = y;
	result[2][3] = z;

	return result;
}

Matrix4 Matrix4::RotationX(float radians)
{
	Matrix4 result = Matrix4::Identity();
	float cosOfAngle = cosf(radians);
	float sinOfAngle = sinf(radians);

	result[1][1] = cosOfAngle;
	result[1][2] = -sinOfAngle;
	result[2][1] = sinOfAngle;
	result[2][2] = cosOfAngle;

	return result;
}

Matrix4 Matrix4::RotationY(float radians)
{
	Matrix4 result = Matrix4::Identity();
	float cosOfAngle = cosf(radians);
	float sinOfAngle = sinf(radians);

	result[0][0] = cosOfAngle;
	result[0][2] = sinOfAngle;
	result[2][0] = -sinOfAngle;
	result[2][2] = cosOfAngle;

	return result;
}

Matrix4 Matrix4::RotationZ(float radians)
{
	Matrix4 result = Matrix4::Identity();
	float cosOfAngle = cosf(radians);
	float sinOfAngle = sinf(radians);

	result[0][0] = cosOfAngle;
	result[0][1] = -sinOfAngle;
	result[1][0] = sinOfAngle;
	result[1][1] = cosOfAngle;

	return result;
}

Matrix4 Matrix4::Rotation(float xRadians, float yRadians, float zRadians)
{
	return RotationX(xRadians) * RotationY(yRadians) * RotationZ(zRadians);
}

Matrix4 Matrix4::RotationXByDegree(float degree)
{
	return RotationX(ConvertDegreeToRadians(degree));
}

Matrix4 Matrix4::RotationYByDegree(float degree)
{
	return RotationY(ConvertDegreeToRadians(degree));
}

Matrix4 Matrix4::RotationZByDegree(float degree)
{
	return RotationZ(ConvertDegreeToRadians(degree));
}

Matrix4 Matrix4::RotationByDegree(float xDegree, float yDegree, float zDegree)
{
	return RotationXByDegree(xDegree) * RotationYByDegree(yDegree) * RotationZByDegree(zDegree);
}
