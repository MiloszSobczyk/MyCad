#pragma once

#include "Algebra.h"
#include <nlohmann/json_fwd.hpp>

using json = nlohmann::json;

class Serializer
{
public:
	static json Serialize(const Algebra::Quaternion& q);
	static json Serialize(const Algebra::Vector4& v);
	
	static Algebra::Quaternion DeserializeQuaternion(const json& j);
	static Algebra::Vector4 DeserializeVector4(const json& j);
};