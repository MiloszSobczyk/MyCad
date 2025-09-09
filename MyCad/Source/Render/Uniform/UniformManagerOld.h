#pragma once

#include "UniformOld.h"
#include "Utils/Base.h"

#include <unordered_map>
#include <iostream>

class UniformManagerOld {
public:
    static UniformManagerOld& GetInstance()
    {
        static UniformManagerOld instance;
        return instance;
    }

    template<typename T>
    void SetUniformValue(const std::string& name, const T& value);

    template<typename T>
    const T* GetUniformValue(const std::string& name) const;

private:
    UniformManagerOld() = default;
    ~UniformManagerOld() = default;
    UniformManagerOld(const UniformManagerOld&) = delete;
    UniformManagerOld& operator=(const UniformManagerOld&) = delete;

    std::unordered_map<std::string, Scope<IUniform>> m_Uniforms;
};

#include "UniformManagerOld.ipp"
#include "UniformManagerOld.h"
