#pragma once

#include "BaseRenderer.h"

#include <stdexcept>
#include <typeinfo>

template<typename TVertex>
BaseRenderer<TVertex>::BaseRenderer(const std::vector<TVertex>& vertices,
    const std::vector<unsigned int>& indices)
{
    vao = std::make_unique<VertexArray>();
    vbo = std::make_unique<VertexBuffer>(vertices);
    ibo = std::make_unique<IndexBuffer>(indices);

    vao->Bind();
    vbo->Bind();
    ibo->Bind();

    const auto* layout = VertexLayoutRegistry::Get(std::type_index(typeid(TVertex)));
    if (layout)
    {
        layout->SetupVAO();
    }
    else
    {
        throw std::runtime_error("Missing layout for vertex type");
    }

    vao->Unbind();
}

template<typename TVertex>
void BaseRenderer<TVertex>::Render() 
{
    vao->Bind();
    if (ibo->GetCount() > 0)
    {
        glDrawElements(GL_TRIANGLES, ibo->GetCount(), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, vbo->GetCount());
    }
}
