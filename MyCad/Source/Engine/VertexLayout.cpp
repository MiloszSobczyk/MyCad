#include "VertexLayout.h"

void VertexLayout::EnableAttribArrays() const
{
	GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PositionColorVertex), (void*)offsetof(PositionColorVertex, point)));
	GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PositionColorVertex), (void*)offsetof(PositionColorVertex, color)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}
