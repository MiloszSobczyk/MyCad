#include "AxisCursor.h"

std::vector<PositionVertexData> AxisCursor::vertices = {
	{{ 0.f, 0.f, 0.f, 1.f }},
	{{ 1.f, 0.f, 0.f, 1.f }},
	{{ 0.f, 1.f, 0.f, 1.f }},
	{{ 0.f, 0.f, 1.f, 1.f }},
};

std::vector<unsigned int> AxisCursor::indices = {
	0, 1,
	0, 2,
	0, 3,
};

AxisCursor::AxisCursor()
	: visible(true), size(1.f), position(0.f, 0.f, 0.f), color(1.f, 0.f, 0.f, 1.f), renderer(VertexDataType::PositionVertexData)
{
	renderer.SetVertices(vertices);
	renderer.SetIndices(indices);
}

void AxisCursor::SetPosition(Algebra::Vector4 position)
{
	this->position = position;
}

void AxisCursor::SetSize(float size)
{
	this->size = size;
}

void AxisCursor::SetColor(Algebra::Vector4 color)
{
	this->color = color;
}

void AxisCursor::SetVisible(bool visible)
{
	this->visible = visible;
}

void AxisCursor::Render()
{
	renderer.Render(GL_LINES);
}