#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Utils/Base.h"

#include <vector>
#include <memory>
#include <cstdint>

class VertexArray
{
public:
	VertexArray();
	~VertexArray();

	void Bind() const;
	void Unbind() const;

	void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer);
	void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer);

	const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const { return m_VertexBuffers; }
	const Ref<IndexBuffer>& GetIndexBuffer() const { return m_IndexBuffer; }

private:
	uint32_t m_RendererID = 0;
	uint32_t m_VertexBufferIndex = 0;
	std::vector<Ref<VertexBuffer>> m_VertexBuffers;
	Ref<IndexBuffer> m_IndexBuffer;
};
