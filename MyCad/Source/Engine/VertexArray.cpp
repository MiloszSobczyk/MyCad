#include "VertexArray.h"
#include <Utils/Logger.h>

VertexArray::VertexArray(VertexBuffer vbo, const VertexLayout& layout)
	: id(0), vbo(vbo)
{
	GLCall(glGenVertexArrays(1, &id));
	Bind();
	vbo.Bind();
	layout.EnableAttribArrays(PositionColor);
	Unbind();
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(id));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}