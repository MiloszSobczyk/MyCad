#include "OpenGLApi.h"

#include "Render/Buffer/VertexArray.h"
#include "Utils/Logger.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

#include <iostream>

bool OpenGLApi::InitGLFW()
{
	return glfwInit();
}

bool OpenGLApi::InitGLEW()
{
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Cannot initiate glew" << std::endl;
		return false;
	}

	return true;
}

void OpenGLApi::InitRendering()
{
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	// TODO: re-add it
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glPatchParameteri(GL_PATCH_VERTICES, 4));
}

void OpenGLApi::DeinitGLFW()
{
	GLCall(glfwTerminate());
}

void OpenGLApi::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
	GLCall(glViewport(x, y, width, height));
}

void OpenGLApi::SetClearColor(const Algebra::Vector4& color)
{
	GLCall(glClearColor(color.x, color.y, color.z, color.w));
}

void OpenGLApi::Clear()
{
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
}

void OpenGLApi::Draw(const Ref<VertexArray>& vertexArray, int renderingMode)
{
	vertexArray->Bind();
	GLCall(glDrawElements(GL_LINES, vertexArray->GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, nullptr));
	vertexArray->Unbind();
}
