#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <GL/glew.h>

#include "Shader.h"

enum class ShaderType
{
    Vertex,
    Fragment,
    TessControl,
    TessEvaluation,
    Geometry,
    Compute
};

struct ShaderTypeInfo
{
    GLenum glType;
    std::string extension;

    static const std::unordered_map<ShaderType, ShaderTypeInfo>& GetMap()
    {
        static std::unordered_map<ShaderType, ShaderTypeInfo> map = {
            { ShaderType::Vertex,         { GL_VERTEX_SHADER,         ".vert" } },
            { ShaderType::Fragment,       { GL_FRAGMENT_SHADER,       ".frag" } },
            { ShaderType::TessControl,    { GL_TESS_CONTROL_SHADER,   ".tesc"  } },
            { ShaderType::TessEvaluation, { GL_TESS_EVALUATION_SHADER,".tese"  } },
            { ShaderType::Geometry,       { GL_GEOMETRY_SHADER,       ".geom" } },
            { ShaderType::Compute,        { GL_COMPUTE_SHADER,        ".comp" } }
        };
        return map;
    }

    static const ShaderTypeInfo& Get(ShaderType type)
    {
        return GetMap().at(type);
    }
};

class ShaderBuilder
{
private:
    std::string m_ShaderDirectory = "Resources/Shaders/";
    std::unordered_map<ShaderType, std::string> m_ShaderSources;

    std::string LoadSourceFromFile(const std::string& filepath);
    unsigned int CompileShader(ShaderType type, const std::string& source);

public:
    ShaderBuilder& SetShaderDirectory(const std::string& directory);
    ShaderBuilder& AddShader(ShaderType type, const std::string& name);

    std::shared_ptr<Shader> Compile();
};
