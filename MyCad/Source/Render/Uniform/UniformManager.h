#pragma once

#include "Uniform.h"
#include "Utils/Base.h"

#include <unordered_map>
#include <iostream>

class UniformManager {
public:
    static UniformManager& GetInstance()
    {
        static UniformManager instance;
        return instance;
    }

    template<typename T>
    void RegisterUniform(const std::string& name, const T& value);

    template<typename T>
    void SetUniformValue(const std::string& name, const T& value);

    template<typename T>
    const T* GetUniformValue(const std::string& name) const;

private:
    UniformManager() = default;
    ~UniformManager() = default;
    UniformManager(const UniformManager&) = delete;
    UniformManager& operator=(const UniformManager&) = delete;

    std::unordered_map<std::string, Scope<IUniform>> m_Uniforms;
};

#include "UniformManager.ipp"
