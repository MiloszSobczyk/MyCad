#include "Renderer.h"

#include "Utils/Logger.h"

void Renderer::Render(RenderingMode renderingMode)
{
	if (!m_Shader || !m_VertexArray)
		return;

	m_Shader->Bind();
	m_Shader->SetUniformMat4f("u_modelMatrix", m_ModelMatrix);
	m_Shader->SetUniformMat4f("u_viewMatrix", m_ViewMatrix);
	m_Shader->SetUniformMat4f("u_projectionMatrix", m_ProjectionMatrix);
	
	m_VertexArray->Bind();
	
	GLCall(glDrawElements(static_cast<GLenum>(renderingMode), m_VertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
	
	m_VertexArray->Unbind();
	m_Shader->Unbind();
}