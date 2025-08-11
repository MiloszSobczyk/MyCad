#pragma once

#include "Utils/Base.h"

#include "Algebra.h"

class VertexArray;

class Renderer
{
public:
	static void SetClearColor(const Algebra::Vector4& color);
	static void Clear();

	static void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0);
	static void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t vertexCount);
};