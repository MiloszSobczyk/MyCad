#pragma once
#include "VertexBuffer.h"
#include "VertexLayout.h"
class VertexArray
{
private:
	unsigned int id;
	VertexBuffer vbo;

public:
	VertexArray(VertexBuffer vbo, const VertexLayout& layout);

	void Bind() const;
	void Unbind() const;
};
