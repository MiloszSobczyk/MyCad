#include "Matrix4.h"

#include <stdexcept>
#include "Utils.h"

using namespace Algebra;

Matrix4::Matrix4()
{
	rows[0] = Vector4();
	rows[1] = Vector4();
	rows[2] = Vector4();
	rows[3] = Vector4();
}

Matrix4::Matrix4(const Vector4& diagonal)
	: Matrix4()
{
	rows[0][0] = diagonal[0];
	rows[1][1] = diagonal[1];
	rows[2][2] = diagonal[2];
	rows[3][3] = diagonal[3];
}

Algebra::Matrix4::Matrix4(const float& v1, const float& v2, const float& v3, const float& v4)
	: Matrix4(Vector4(v1, v2, v3, v4))
{
}

Matrix4::Matrix4(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4)
{
	rows[0] = row1;
	rows[1] = row2;
	rows[2] = row3;
	rows[3] = row4;
}

float Matrix4::Sum() const
{
	float result = 0.f;
	for (int i = 0; i < 4; i++)
		result += rows[i].Sum();
	return result;
}

Matrix4 Algebra::Matrix4::Transpose()
{
	return Matrix4(this->Column(0), this->Column(1), this->Column(2), this->Column(3));
}

Vector4 Matrix4::Column(int index) const
{
	return Vector4(rows[0][index], rows[1][index], rows[2][index], rows[3][index]);
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
	return Matrix4(rows[0] + other[0], rows[1] + other[1], rows[2] + other[2], rows[3] + other[3]);
}

const Matrix4 Matrix4::operator-(const Matrix4& other) const
{
	return Matrix4(rows[0] - other[0], rows[1] - other[1], rows[2] - other[2], rows[3] - other[3]);
}

const Matrix4 Algebra::Matrix4::operator*(const Matrix4& matrix) const
{
	Matrix4 result;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			result[i][j] = (*this)[i] * matrix.Column(j);
		}
	}
	return result;
}

std::ostream& Algebra::operator<<(std::ostream& os, const Matrix4& matrix)
{
	for (int i = 0; i < 4; ++i)
	{
		os << matrix.rows[i] << '\n';
	}
	return os;
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

Matrix4 Algebra::Matrix4::Identity()
{
	return Matrix4(1, 1, 1, 1);
}

Matrix4 Algebra::Matrix4::Translation(float x, float y, float z)
{
	Matrix4 result = Matrix4::Identity();
	result[0][3] = x;
	result[1][3] = y;
	result[2][3] = z;
	return result;
}

Matrix4 Algebra::Matrix4::Translation(Vector4 translation)
{
	return Translation(translation.x, translation.y, translation.z);
}

Matrix4 Algebra::Matrix4::Rotation(float x, float y, float z)
{
	return RotationX(x) * RotationY(y) * RotationZ(z);
}

Matrix4 Algebra::Matrix4::Rotation(Vector4 rotation)
{
	return Rotation(rotation.x, rotation.y, rotation.z);
}

Matrix4 Algebra::Matrix4::RotationX(float angle)
{
	Matrix4 result = Matrix4::Identity();
	float cosOfAngle = cosf(angle);
	float sinOfAngle = sinf(angle);

	result[1][1] = cosOfAngle;
	result[1][2] = -sinOfAngle;
	result[2][1] = sinOfAngle;
	result[2][2] = cosOfAngle;

	return result;
}

Matrix4 Algebra::Matrix4::RotationY(float angle)
{
	Matrix4 result = Matrix4::Identity();
	float cosOfAngle = cosf(angle);
	float sinOfAngle = sinf(angle);

	result[0][0] = cosOfAngle;
	result[0][2] = sinOfAngle;
	result[2][0] = -sinOfAngle;
	result[2][2] = cosOfAngle;

	return result;
}

Matrix4 Algebra::Matrix4::RotationZ(float angle)
{
	Matrix4 result = Matrix4::Identity();
	float cosOfAngle = cosf(angle);
	float sinOfAngle = sinf(angle);

	result[0][0] = cosOfAngle;
	result[0][1] = -sinOfAngle;
	result[1][0] = sinOfAngle;
	result[1][1] = cosOfAngle;

	return result;
}

Matrix4 Algebra::Matrix4::RotationByDegree(float x, float y, float z)
{
	return RotationXByDegree(x) * RotationYDegree(y) * RotationZDegree(z);
}

Matrix4 Algebra::Matrix4::RotationByDegree(Vector4 rotation)
{
	return RotationByDegree(rotation.x, rotation.y, rotation.z);
}

Matrix4 Algebra::Matrix4::RotationXByDegree(float angle)
{
	return RotationX(DegreeToRadians(angle));
}

Matrix4 Algebra::Matrix4::RotationYDegree(float angle)
{
	return RotationY(DegreeToRadians(angle));
}

Matrix4 Algebra::Matrix4::RotationZDegree(float angle)
{
	return RotationZ(DegreeToRadians(angle));
}

Matrix4 Algebra::Matrix4::Scale(float x, float y, float z)
{
	return Matrix4(x, y, z, 1.f);
}

Matrix4 Algebra::Matrix4::Scale(Vector4 scale)
{
	return Matrix4(scale.x, scale.y, scale.z, 1.f);
}

Matrix4 Algebra::Matrix4::Projection(float aspect, float f, float n, float fov)
{
	float ctgFov = cosf(fov / 2.f) / sinf(fov / 2.f);
	auto result = Matrix4(ctgFov / aspect, ctgFov, (f + n) / (f - n), 0.f);

	result[3][2] = 1.f;
	result[2][3] = (- 2.f * n * f) / (f - n);

	return result;
}

StereoscopicMatrices Matrix4::StereoscopicProjection(float aspect, float f, float n, float fov, float d, float c)
{
	float top = n * tanf(fov * 0.5f);
	float bottom = -top;
	float right = top * aspect;
	float left = -right;

	float shift = (d * 0.5f) * n / c;
	float lL = left + shift;
	float rL = right + shift;
	float lR = left - shift;
	float rR = right - shift;

	auto makeOffAxis = [&](float l, float r) {
		Matrix4 P;

		P[0][0] = 2.0f * n / (r - l);
		P[0][2] = (r + l) / (r - l);

		P[1][1] = 2.0f * n / (top - bottom);
		P[1][2] = (top + bottom) / (top - bottom);

		P[2][2] = (f + n) / (f - n);
		P[2][3] = -2.0f * n * f / (f - n);

		P[3][2] = 1.0f;
		return P;
	};

	StereoscopicMatrices matrices;
	matrices.left = makeOffAxis(lL, rL) * Translation(-d * 0.5f, 0.f, 0.f);
	matrices.right = makeOffAxis(lR, rR) * Translation(d * 0.5f, 0.f, 0.f);;
	
	return matrices;
}
