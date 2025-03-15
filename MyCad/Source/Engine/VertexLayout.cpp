#include "VertexLayout.h"


VertexLayout::VertexLayout(VertexDataType dataType)
	: dataType(dataType)
{}

void VertexLayout::EnableAttribArrays() const
{
	switch (dataType)
	{
	case VertexDataType::Position:
		GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PositionVertexData), 
			(void*)offsetof(PositionVertexData, Position)));
		GLCall(glEnableVertexAttribArray(0));
		break;
	case VertexDataType::PositionColor:
		GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PositionColorVertexData), 
			(void*)offsetof(PositionColorVertexData, Position)));
		GLCall(glEnableVertexAttribArray(0));		
		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PositionColorVertexData), 
			(void*)offsetof(PositionColorVertexData, Color)));
		GLCall(glEnableVertexAttribArray(1));
		break;
	case VertexDataType::PositionNormal:
		GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(PositionNormalVertexData),
			(void*)offsetof(PositionNormalVertexData, Position)));
		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(PositionNormalVertexData),
			(void*)offsetof(PositionNormalVertexData, Normal)));
		GLCall(glEnableVertexAttribArray(1));
		break;
	default:
		throw std::runtime_error("Invalid vertex data type");
		break;
	}
}