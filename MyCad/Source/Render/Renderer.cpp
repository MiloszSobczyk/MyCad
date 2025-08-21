#include "Renderer.h"

#include "Utils/Logger.h"
#include "Utils/Config.h"

void Renderer::Render(RenderingMode renderingMode)
{
	if (!m_Shader || !m_VertexArray)
		return;

	m_Shader->Bind();

    for (auto& [name, value] : m_Uniforms)
    {
        std::visit([&](auto&& val)
            {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, int>)
                    m_Shader->SetUniformInt(name, val);
                else if constexpr (std::is_same_v<T, Algebra::Matrix4>)
                    m_Shader->SetUniformMat4f(name, val);
                else if constexpr (std::is_same_v<T, Algebra::Vector4>)
                    m_Shader->SetUniformVec4f(name, val);
            }, value);
    }
	
	m_VertexArray->Bind();
	
    if (auto& indexBuffer = m_VertexArray->GetIndexBuffer())
    {
        GLCall(glDrawElements(
            static_cast<GLenum>(renderingMode),
            indexBuffer->GetCount(),
            GL_UNSIGNED_INT,
            nullptr
        ));
    }
    else if (!m_VertexArray->GetVertexBuffers().empty())
    {
        auto& vertexBuffer = m_VertexArray->GetVertexBuffers()[0];
        uint32_t vertexCount = vertexBuffer->GetVertexCount();

        GLCall(glDrawArrays(
            static_cast<GLenum>(renderingMode),
            0,
            vertexCount
        ));
    }
	
	m_VertexArray->Unbind();
	m_Shader->Unbind();
}
