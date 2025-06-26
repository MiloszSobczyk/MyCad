#pragma once

#include "VertexBuffer.h"
#include "VertexLayout.h"
#include "IndexBuffer.h"

class VertexArray
{
private:
	unsigned int id;
	VertexDataType dataType;
	
	void EnableLayout();

public:
	VertexArray(const VertexBuffer& vbo, VertexDataType dataType);
	VertexArray(const VertexBuffer& vbo, VertexDataType dataType, const IndexBuffer& ibo);
	~VertexArray();

	void Bind() const;
	void Unbind() const;
};