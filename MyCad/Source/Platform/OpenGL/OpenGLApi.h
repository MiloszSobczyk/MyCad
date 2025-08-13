#pragma once

#include "Utils/Base.h"

#include "Algebra.h"

class VertexArray;

class OpenGLApi
{
// Initialization
public:
	static bool InitGLFW();
	static bool InitGLEW();
	static void InitRendering();

// Deinitialization
public:
	static void DeinitGLFW();

// Rendering
public:
	static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
	
	static void SetClearColor(const Algebra::Vector4& color);
	static void Clear();
	
	// scrapped for now
	static void Draw(const Ref<VertexArray>& vertexArray, int renderingMode);
};