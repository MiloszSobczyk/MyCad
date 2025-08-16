#pragma once 

#include <string>

struct NameComponent
{
	std::string name;
	NameComponent(const std::string& name = "")
		: name(name)
	{
	}
};