#include "Renderer.h"

#include "Platform/OpenGL/OpenGLApi.h"

void Renderer::SetClearColor(const Algebra::Vector4& color)
{
	OpenGLApi::SetClearColor(color);
}

void Renderer::Clear()
{
	OpenGLApi::Clear();
}

void Renderer::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
{
	OpenGLApi::DrawIndexed(vertexArray, indexCount);
}

void Renderer::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount)
{
	OpenGLApi::DrawLines(vertexArray, vertexCount);
}