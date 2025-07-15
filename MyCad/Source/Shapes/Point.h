#pragma once

#include "Render/Renderers/BaseRenderer.h"
#include "Render/VertexData/VertexLayoutTypes.h"
#include "Render/Shader/ShaderManager.h"

#include <memory>

class Point 
{
public:
    void Render();

private:
    std::unique_ptr<BaseRenderer<PositionVertex>> renderer;
};
