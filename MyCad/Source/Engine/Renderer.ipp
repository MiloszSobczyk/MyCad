#pragma once
#include "Renderer.h"

template<class T>
Renderer<T>::Renderer(VertexDataType dataType)
	:vbo(), ibo(), vao(vbo, dataType, ibo)
{
}

template<class T>
Renderer<T>::Renderer(VertexDataType dataType, std::vector<unsigned int> indices)
	: vbo(), ibo(indices), vao(vbo, dataType, ibo)
{
}

template<class T>
Renderer<T>::Renderer(VertexDataType dataType, std::vector<T> vertices)
	:vbo(vertices), ibo(), vao(vbo, dataType, ibo)
{
}

template<class T>
Renderer<T>::Renderer(VertexDataType dataType, std::vector<T> vertices, std::vector<unsigned int> indices)
	:vbo(vertices), ibo(indices), vao(vbo, dataType, ibo)
{
}

template<class T>
void Renderer<T>::SetIndices(std::vector<unsigned int> indices)
{
	ibo.SetIndices(indices);
}

template<class T>
void Renderer<T>::SetVertices(std::vector<T> vertices)
{
	vbo.SetVertices(vertices);
}

template<class T>
void Renderer<T>::Render(GLenum mode)
{
	vao.Bind();
	if (ibo.GetCount() > 0)
	{
		GLCall(glDrawElements(mode, static_cast<unsigned int>(ibo.GetCount()), GL_UNSIGNED_INT, nullptr));
	}
	else
	{
		GLCall(glDrawArrays(mode, 0, static_cast<unsigned int>(vbo.GetCount())));
	}

	vao.Unbind();
}
