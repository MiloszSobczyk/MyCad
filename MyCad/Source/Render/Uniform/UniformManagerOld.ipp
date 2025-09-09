template<typename T>
void UniformManagerOld::SetUniformValue(const std::string& name, const T& value)
{
    auto it = m_Uniforms.find(name);

    if (it == m_Uniforms.end())
    {
        m_Uniforms[name] = CreateScope<UniformOld<T>>(name, value);
        return;
    }

    auto uniform = dynamic_cast<UniformOld<T>*>(it->second.get());
    if (!uniform)
    {
        std::cerr << "Uniform '" << name << "' has different type.\n";
        return;
    }

    uniform->SetValue(value);
}


template<typename T>
const T* UniformManagerOld::GetUniformValue(const std::string& name) const
{
    auto it = m_Uniforms.find(name);
    if (it == m_Uniforms.end()) 
    {
        std::cerr << "Uniform '" << name << "' not found.\n";
        return nullptr;
    }

    auto uniform = dynamic_cast<UniformOld<T>*>(it->second.get());
    if (!uniform) 
    {
        std::cerr << "Uniform '" << name << "' has different type.\n";
        return nullptr;
    }

    return &(uniform->GetValue());
}