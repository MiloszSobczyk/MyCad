#include "VertexArray.h"
#include <utils/GlCall.h>

void VertexArray::EnableLayout()
{
	switch (dataType)
	{
	case VertexDataType::PositionVertexData:
		GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PositionVertexData),
			(void*)offsetof(PositionVertexData, Position)));
		GLCall(glEnableVertexAttribArray(0));
		break;
	case VertexDataType::PositionColorVertexData:
		GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PositionColorVertexData),
			(void*)offsetof(PositionColorVertexData, Position)));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PositionColorVertexData),
			(void*)offsetof(PositionColorVertexData, Color));
		GLCall(glEnableVertexAttribArray(1)));
		break;
	case VertexDataType::PositionNormalVertexData:
		GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PositionNormalVertexData),
			(void*)offsetof(PositionNormalVertexData, Position)));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PositionNormalVertexData),
			(void*)offsetof(PositionNormalVertexData, Normal)));
		GLCall(glEnableVertexAttribArray(1));
		break;
	default:
		throw std::runtime_error("Invalid Vertex Data type");
		break;
	}
}

VertexArray::VertexArray(const VertexBuffer& vbo, VertexDataType dataType)
	: id(0), dataType(dataType)
{
	GLCall(glGenVertexArrays(1, &id));
	Bind();
	vbo.Bind();
	EnableLayout();
	Unbind();
	vbo.Unbind();
}

VertexArray::VertexArray(const VertexBuffer& vbo, VertexDataType dataType, const IndexBuffer& ibo)
	: id(0), dataType(dataType)
{
	GLCall(glGenVertexArrays(1, &id));
	Bind();
	vbo.Bind();
	ibo.Bind();
	EnableLayout();
	Unbind();
	vbo.Unbind();
	ibo.Unbind();
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &id);
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(id));
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0));
}