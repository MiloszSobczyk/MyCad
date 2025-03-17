#pragma once
#include <vector>
#include <utils/GlCall.h>
#include <iostream>

class VertexBuffer
{
private:
	int count;
public:
	VertexBuffer();
	template<class T> VertexBuffer(std::vector<T> vertices);
	~VertexBuffer();

	int GetCount() const;

	void Bind() const;
	void Unbind() const;

	template<class T> void SetVertices(std::vector<T> vertices);
private:
	unsigned int id;
};

template<class T>
inline VertexBuffer::VertexBuffer(std::vector<T> vertices)
	: VertexBuffer()
{
	SetVertices(vertices);
}

template<class T>
inline void VertexBuffer::SetVertices(std::vector<T> vertices)
{
	count = vertices.size();
	Bind();
	GLCall(glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_DYNAMIC_DRAW));
	Unbind();
}
