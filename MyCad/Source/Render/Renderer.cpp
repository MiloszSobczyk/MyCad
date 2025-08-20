#include "Renderer.h"

#include "Utils/Logger.h"
#include "Utils/Config.h"

void Renderer::Render(RenderingMode renderingMode)
{
	if (!m_Shader || !m_VertexArray)
		return;

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_viewMatrix", m_ViewMatrix);
	m_Shader->SetUniformMat4f("u_projectionMatrix", m_ProjectionMatrix);
	m_Shader->SetUniformVec4f("u_cameraPos", Config::InitialCameraPosition);
	
	m_VertexArray->Bind();
	
    if (auto& indexBuffer = m_VertexArray->GetIndexBuffer())
    {
        // Indexed drawing
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
