#pragma once

#include "Buffer/VertexArray.h"
#include "Shader/Shader.h"
#include "Utils/Base.h"

#include "Algebra.h"

#include <GL/glew.h>

enum class RenderingMode
{
	Lines = GL_LINES,
	Triangles = GL_TRIANGLES,
	Patches = GL_PATCHES,
};

class Renderer
{
public:
	void Render(RenderingMode renderingMode);

	inline void SetShader(const Ref<Shader>& shader) { m_Shader = shader; }
	inline void SetVertexArray(const Ref<VertexArray>& vertexArray) { m_VertexArray = vertexArray; }
	inline void SetProjectionMatrix(Algebra::Matrix4 projectionMatrix) { m_ProjectionMatrix = projectionMatrix; }
	inline void SetViewMatrix(Algebra::Matrix4 viewMatrix) { m_ViewMatrix = viewMatrix; }
	inline void SetModelMatrix(Algebra::Matrix4 modelMatrix) { m_ModelMatrix = modelMatrix; }

private:
	Ref<Shader> m_Shader;
	Ref<VertexArray> m_VertexArray;
	Algebra::Matrix4 m_ProjectionMatrix;
	Algebra::Matrix4 m_ViewMatrix;
	Algebra::Matrix4 m_ModelMatrix;
};