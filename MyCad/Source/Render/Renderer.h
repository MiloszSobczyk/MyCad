#pragma once

#include "Buffer/VertexArray.h"
#include "Shader/Shader.h"
#include "Utils/Base.h"

#include "Algebra.h"

#include <GL/glew.h>
#include <variant>

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
	
	template<typename T>
	inline void SetUniform(const std::string& name, const T& value) { m_Uniforms[name] = value; }

	inline void ClearUniforms() { m_Uniforms.clear(); }
private:
	Ref<Shader> m_Shader;
	Ref<VertexArray> m_VertexArray;

	using UniformValue = std::variant<
		int,
		Algebra::Matrix4,
		Algebra::Vector4 
	>;

	std::unordered_map<std::string, UniformValue> m_Uniforms;
};