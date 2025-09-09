#include <string>

class IUniform 
{
public:
    virtual ~IUniform() = default;
};

template<typename T>
class UniformOld : public IUniform
{
public:
    inline UniformOld(const std::string& name, const T& value)
        : m_Name(name), m_Value(value) {
    }

    inline void SetValue(const T& value) { m_Value = value; }
    inline const T& GetValue() const { return m_Value; }

private:
    std::string m_Name;
    T m_Value;
};