#pragma once

#include "VertexArray.h"

template <class T> 
class Renderer
{
private:
	VertexBuffer vbo;
	IndexBuffer ibo;
	VertexArray vao;

public:
	Renderer(VertexDataType dataType);
	Renderer(VertexDataType dataType, std::vector<unsigned int> indices);
	Renderer(VertexDataType dataType, std::vector<T> vertices);
	Renderer(VertexDataType dataType, std::vector<T> vertices, std::vector<unsigned int> indices);

	void SetVertices(std::vector<T> vertices);
	void SetIndices(std::vector<unsigned int> indices);

	void Render(GLenum mode);
};

#include "Renderer.ipp"