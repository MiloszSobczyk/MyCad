#pragma once

#include "Utils/Base.h"
#include "Render/Buffer/VertexArray.h"
#include "Render/Renderer.h"

struct MeshComponent
{
	Ref<VertexArray> vertexArray;
	RenderingMode renderingMode;
};