#pragma once
#include <vector>
#include <Utils/Logger.h>
#include "VertexLayout.h"

class VertexBuffer
{
private:
	unsigned int id;

public:
	VertexBuffer();
	VertexBuffer(std::vector<PositionColorVertex> vertices);

	void Bind() const;
	void Unbind() const;

	void SetVertices(std::vector<PositionColorVertex> vertices) const;
};
