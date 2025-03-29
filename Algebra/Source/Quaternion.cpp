#include "Quaternion.h"

namespace Algebra 
{
    float Quaternion::Length() const 
    {
        return std::sqrt(w * w + x * x + y * y + z * z);
    }

    Quaternion Quaternion::Normalize() const 
    {
        float len = Length();

        if (len <= 0.0f) 
        {
            return Quaternion::Identity();
        }

        return Quaternion(w / len, x / len, y / len, z / len);
    }

    Quaternion Quaternion::Conjugate() const 
    {
        return Quaternion(w, -x, -y, -z);
    }

    Quaternion Quaternion::Inverse() const 
    {
        float lenSq = w * w + x * x + y * y + z * z;
        if (lenSq == 0.f) 
        {
            throw std::runtime_error("Cannot invert a zero-length quaternion.");
        }
        return Conjugate() * (1.0f / lenSq);
    }

    float Quaternion::Dot(const Quaternion& other) const 
    {
        return w * other.w + x * other.x + y * other.y + z * other.z;
    }

    Matrix4 Quaternion::ToMatrix() const 
    {
        float xx = x * x;
        float xy = x * y;
        float xz = x * z;
        float xw = x * w;
        float yy = y * y;
        float yz = y * z;
        float yw = y * w;
        float zz = z * z;
        float zw = z * w;

        Matrix4 mat;

        mat[0][0] = 1.0f - 2.0f * (yy + zz);
        mat[0][1] = 2.0f * (xy + zw);
        mat[0][2] = 2.0f * (xz - yw);
        mat[0][3] = 0.0f;

        mat[1][0] = 2.0f * (xy - zw);
        mat[1][1] = 1.0f - 2.0f * (xx + zz);
        mat[1][2] = 2.0f * (yz + xw);
        mat[1][3] = 0.0f;

        mat[2][0] = 2.0f * (xz + yw);
        mat[2][1] = 2.0f * (yz - xw);
        mat[2][2] = 1.0f - 2.0f * (xx + yy);
        mat[2][3] = 0.0f;

        mat[3][0] = 0.0f;
        mat[3][1] = 0.0f;
        mat[3][2] = 0.0f;
        mat[3][3] = 1.0f;

        return mat;
    }

    Vector4 Quaternion::Rotate(const Vector4& vector) const 
    {
        Quaternion vecQuat(0.0f, vector.x, vector.y, vector.z);
        Quaternion result = (*this) * vecQuat * this->Conjugate();
        return Vector4(result.x, result.y, result.z, vector.w);
    }

    Quaternion Quaternion::operator*(const Quaternion& other) const 
    {
        return Quaternion(
            w * other.w - x * other.x - y * other.y - z * other.z,
            w * other.x + x * other.w + y * other.z - z * other.y,
            w * other.y - x * other.z + y * other.w + z * other.x,
            w * other.z + x * other.y - y * other.x + z * other.w
        );
    }

    Quaternion Quaternion::operator+(const Quaternion& other) const 
    {
        return Quaternion(w + other.w, x + other.x, y + other.y, z + other.z);
    }

    Quaternion Quaternion::operator*(float scalar) const 
    {
        return Quaternion(w * scalar, x * scalar, y * scalar, z * scalar);
    }

    Quaternion& Quaternion::operator*=(float scalar) 
    {
        w *= scalar;
        x *= scalar;
        y *= scalar;
        z *= scalar;

        return *this;
    }

    bool Quaternion::operator==(const Quaternion& other) const 
    {
        return w == other.w && x == other.x && y == other.y && z == other.z;
    }

    std::ostream& operator<<(std::ostream& os, const Quaternion& quaternion)
    {
        os << "[" << quaternion.w << ' ' << quaternion.x << ' ' << quaternion.y << ' ' << quaternion.z << "]\n";
        return os;
    }

    Quaternion operator*(float scalar, const Quaternion& q)
    {
        return q * scalar;
    }

    Quaternion Quaternion::CreateFromAxisAngle(const Vector4& axis, float angle) 
    {
        float halfAngle = angle * 0.5f;
        float sinHalf = std::sin(halfAngle);
        Vector4 normalized = axis.Normalize();

        return Quaternion(
            std::cos(halfAngle),
            normalized.x * sinHalf,
            normalized.y * sinHalf,
            normalized.z * sinHalf
        );
    }

    Quaternion Quaternion::CreateFromEulerAngles(float yaw, float pitch, float roll) 
    {
        float cy = std::cos(yaw * 0.5f);
        float sy = std::sin(yaw * 0.5f);
        float cp = std::cos(pitch * 0.5f);
        float sp = std::sin(pitch * 0.5f);
        float cr = std::cos(roll * 0.5f);
        float sr = std::sin(roll * 0.5f);

        return Quaternion(
            cy * cp * cr + sy * sp * sr,
            sy * cp * sr - cy * sp * cr,
            sy * sp * cr + cy * cp * sr,
            cy * sp * sr - sy * cp * cr
        );
    }

    Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t) 
    {
        const float epsilon = 1e-6f;
        float dot = q1.Dot(q2);

        Quaternion q2Adjusted = q2;
        if (dot < 0.0f) 
        {
            q2Adjusted = -1.0f * q2;
            dot = -dot;
        }

        if (dot > 1.0f - epsilon) 
        {
            return (q1 * (1.0f - t) + q2Adjusted * t).Normalize();
        }

        float theta0 = std::acos(dot);
        float theta = theta0 * t;
        float sinTheta = std::sin(theta);
        float sinTheta0 = std::sin(theta0);

        float factor1 = std::cos(theta) - dot * sinTheta / sinTheta0;
        float factor2 = sinTheta / sinTheta0;

        return (q1 * factor1) + (q2Adjusted * factor2);
    }

}