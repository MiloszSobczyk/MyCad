#pragma once

#include "IRenderer.h"
#include "Render/Buffer/Old/VertexArray.h"
#include "Render/Buffer/Old/VertexBuffer.h"
#include "Render/Buffer/Old/IndexBuffer.h"
#include "Render/VertexData/VertexLayout.h"
#include "Algebra.h"

#include <memory>
#include <vector>

template<typename TVertex>
class BaseRenderer : public IRenderer 
{
public:
    BaseRenderer(const std::vector<TVertex>& vertices, const std::vector<unsigned int>& indices);

    void Render() override;

private:
    std::unique_ptr<VertexArray> vao;
    std::unique_ptr<VertexBuffer> vbo;
    std::unique_ptr<IndexBuffer> ibo;
};

#include "BaseRenderer.ipp"