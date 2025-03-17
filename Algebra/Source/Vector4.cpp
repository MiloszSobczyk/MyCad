#include "Vector4.h"
#include <stdexcept>
#include "Matrix4.h"

using namespace Algebra;

Vector4::Vector4()
	: x(0.f), y(0.f), z(0.f), w(0.f)
{
}

Vector4::Vector4(float x, float y, float z, float w)
	:x(x), y(y), z(z), w(w)
{
}

float Algebra::Vector4::Length() const
{
	return sqrtf(x * x + y * y + z * z + w * w);
}

float Algebra::Vector4::Sum() const
{
	return x + y + z + w;
}

const Vector4 Algebra::Vector4::Scale(const Vector4& scale) const
{
	return Vector4(this->x * scale.x, this->y * scale.y,
		this->z * scale.z, this->w * scale.w);
}

Vector4 Algebra::Vector4::Normalize() const
{
	if (Length() == 0)
	{
		return *this;
	}
	return *this / Length();
}

std::vector<float> Algebra::Vector4::ToVector() const
{
	return std::vector<float>{x, y, z, w};
}

float& Vector4::operator[](int index)
{
	switch (index) 
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2: 
		return z;
	case 3:
		return w;
	default:
		throw std::runtime_error("Invalid index");
	}
}

const float& Algebra::Vector4::operator[](int index) const
{
	switch (index)
	{
	case 0:
		return x;
	case 1:
		return y;
	case 2:
		return z;
	case 3:
		return w;
	default:
		throw std::runtime_error("Invalid index");
	}
}

const Vector4 Algebra::Vector4::operator+(const Vector4& other) const
{
	return Vector4(this->x + other.x, this->y + other.y,
		this->z + other.z, this->w + other.w);
}

const Vector4 Algebra::Vector4::operator-(const Vector4& other) const
{
	return Vector4(this->x - other.x, this->y - other.y,
		this->z - other.z, this->w - other.w);
}

const Vector4 Algebra::Vector4::operator-() const
{
	return -1 * (*this);
}

const float Algebra::Vector4::operator*(const Vector4& other) const
{
	return x * other.x + y * other.y + z * other.z + w * other.w;
}

const bool Algebra::Vector4::operator==(const Vector4& v2) const
{
	return x == v2.x && y == v2.y && z == v2.z && w == v2.w;
}

Vector4& Algebra::Vector4::operator+=(const Vector4& other)
{
	x += other.x;
	y += other.y;
	z += other.z;
	w += other.w;

	return *this;
}

Vector4 Algebra::operator*(const Vector4& vector, const Matrix4& matrix)
{
	return Vector4(vector * matrix.Column(0), vector * matrix.Column(1),
		vector * matrix.Column(2), vector * matrix.Column(3));
}

Vector4 Algebra::operator*(const Matrix4& matrix, const Vector4& vector)
{
	return Vector4(matrix[0] * vector, matrix[1] * vector,
		matrix[2] * vector, matrix[3] * vector);
}

Vector4 Algebra::operator*(const Vector4& vector, const float& scalar)
{
	return Vector4(vector.x * scalar, vector.y * scalar,
		vector.z * scalar, vector.w * scalar);
}

Vector4 Algebra::operator/(const Vector4& vector, const float& scalar)
{
	if (scalar == 0)
	{
		throw std::runtime_error("Scalar cannot be zero");
	}
	return 1.f / scalar * vector;
}

std::ostream& Algebra::operator<<(std::ostream& os, const Vector4& vector)
{
	os << '[' << vector.x << ' ' << vector.y << vector.z << vector.w << ']';
	return os;
}

Vector4 Algebra::operator*(const float& scale, const Vector4& vector)
{
	return vector * scale;
}

Vector4 Algebra::Vector4::Cross(Vector4 v1, Vector4 v2)
{
	return Vector4(v1.y * v2.z - v1.z * v2.y, v1.x * v2.z - v1.z * v2.x, v1.x * v2.y - v1.y * v2.x, 0.f);
}
