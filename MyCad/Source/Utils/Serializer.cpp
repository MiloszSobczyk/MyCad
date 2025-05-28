#include "Serializer.h"

json Serializer::Serialize(const Algebra::Quaternion& q)
{
	auto normQ = q.Normalize();
	return json {
		{ "x", normQ.x },
		{ "y", normQ.y },
		{ "z", normQ.z },
		{ "w", normQ.w }
	};
}

json Serializer::Serialize(const Algebra::Vector4& v)
{
	return json {
		{ "x", v.x },
		{ "y", v.y },
		{ "z", v.z },
		{ "w", v.w }
	};
}

Algebra::Quaternion Serializer::DeserializeQuaternion(const json& j)
{
	float x = j.value("x", 0.0f);
	float y = j.value("y", 0.0f);
	float z = j.value("z", 0.0f);
	float w = j.value("w", 1.0f);

	return Algebra::Quaternion(w, x, y, z).Normalize();
}

Algebra::Vector4 Serializer::DeserializeVector4(const json& j)
{
	float x = j.value("x", 0.0f);
	float y = j.value("y", 0.0f);
	float z = j.value("z", 0.0f);

	return Algebra::Vector4(x, y, z);
}
