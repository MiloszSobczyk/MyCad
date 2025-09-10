#pragma once

#include "Utils/Base.h"
#include "Render/Buffer/VertexArray.h"
#include "Render/Renderer.h"
#include "Render/Uniform/UniformCalculation.h"

struct MeshComponent
{
	std::vector<Ref<Shader>> shaders;
	Ref<VertexArray> vertexArray;
	Ref<UniformCalculation> uniformCalculation;
	RenderingMode renderingMode;
};