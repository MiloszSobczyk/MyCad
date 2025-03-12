#include "Vector4.h"
#include <stdexcept>
#include "Matrix4.h"
#include <iostream>

using namespace Algebra;

Vector4::Vector4()
	: x(0), y(0), z(0), w(0)
{}

Vector4::Vector4(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{}

float Vector4::Sum() const
{
	return x + y + z + w;
}

float Vector4::Length() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

Vector4 Vector4::Normalize() const
{
	return *this / Length();
}

Vector4 Algebra::Vector4::Cross(const Vector4& other) const
{
	return Vector4(y * other.z - z * other.y, x * other.z - z * other.x, x * other.y - y * other.x, 0.f);
}

float& Vector4::operator[](int index)
{
	switch (index)
	{
	case 0: return x;
	case 1: return y;
	case 2: return z;
	case 3: return w;
	default: throw std::runtime_error("Index out of bounds");
	}
}

const float& Vector4::operator[](int index) const
{
	switch (index)
	{
	case 0: return x;
	case 1: return y;
	case 2: return z;
	case 3: return w;
	default: throw std::runtime_error("Index out of bounds");
	}
}

const Vector4 Vector4::operator+(const Vector4& other) const
{
	return Vector4(
		this->x + other.x, 
		this->y + other.y,
		this->z + other.z, 
		this->w + other.w
	);
}

const Vector4 Vector4::operator-(const Vector4& other) const
{
	return Vector4(
		this->x - other.x,
		this->y - other.y,
		this->z - other.z,
		this->w - other.w
	);
}

const Vector4 Algebra::Vector4::operator-() const
{
	return Vector4(-x, -y, -z, -w);
}

const float Vector4::operator*(const Vector4& other) const
{
	return x * other.x + y * other.y + z * other.z + w * other.w;
}

const Vector4 Vector4::operator*(const Matrix4& matrix) const
{
	return Vector4(
		(*this) * matrix.Column(0), 
		(*this) * matrix.Column(1),
		(*this) * matrix.Column(2), 
		(*this) * matrix.Column(3)
	);
}

Vector4 Algebra::operator*(const Vector4& vector, const float& scalar)
{
	return Vector4(vector.x * scalar, vector.y * scalar, vector.z * scalar, vector.w * scalar);
}

Vector4 Algebra::operator/(const Vector4& vector, const float& scalar)
{
	if (scalar == 0)
	{
		throw std::runtime_error("Scalar cannot be zero");
	}

	return vector * (1.f /  scalar);
}

Vector4 Algebra::operator*(const float& scalar, const Vector4& vector)
{
	return vector * scalar;
}
