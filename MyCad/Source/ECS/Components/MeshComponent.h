#pragma once

#include "Utils/Base.h"
#include "Render/Buffer/VertexArray.h"
#include "Render/Renderer.h"

struct MeshComponent
{
	std::vector<Ref<Shader>> shaders;
	Ref<VertexArray> vertexArray;
	RenderingMode renderingMode;
};