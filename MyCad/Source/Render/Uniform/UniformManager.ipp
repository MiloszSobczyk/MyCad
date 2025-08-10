template<typename T>
void UniformManager::RegisterUniform(const std::string& name, const T& value)
{
    m_Uniforms[name] = CreateScope<Uniform<T>>(name, value);
}

template<typename T>
void UniformManager::SetUniformValue(const std::string& name, const T& value)
{
    auto it = m_Uniforms.find(name);
    if (it == m_Uniforms.end())
    {
        std::cerr << "Uniform '" << name << "' not found.\n";
        return;
    }

    auto uniform = dynamic_cast<Uniform<T>*>(it->second.get());
    if (!uniform) 
    {
        std::cerr << "Uniform '" << name << "' has different type.\n";
        return;
    }

    uniform->SetValue(value);
}

template<typename T>
const T* UniformManager::GetUniformValue(const std::string& name) const
{
    auto it = m_Uniforms.find(name);
    if (it == m_Uniforms.end()) 
    {
        std::cerr << "Uniform '" << name << "' not found.\n";
        return nullptr;
    }

    auto uniform = dynamic_cast<Uniform<T>*>(it->second.get());
    if (!uniform) 
    {
        std::cerr << "Uniform '" << name << "' has different type.\n";
        return nullptr;
    }

    return &(uniform->GetValue());
}