#include "RayCaster.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Utils/Logger.h"
#include <iostream>
#include <UserInterface.h>

RayCaster::RayCaster() : shader("Resources/Shaders/Shader.glsl")
{
	GLCall(glGenBuffers(1, &VBO));
	GLCall(glGenVertexArrays(1, &VAO));
}

void RayCaster::CalculatePoints(Ellipsoid& shape, Window& window, int raySize)
{
	shape.Refresh();
	points.clear();

	float width = static_cast<float>(window.GetWidth() / 100.f);
	float height = static_cast<float>(window.GetHeight() / 100.f);
	float raySizeF = static_cast<float>(raySize) / 100.f;
	float rayStep = raySizeF / static_cast<float>(raySize);

	for (float x = -width / 2.f; x < width / 2.f; x += raySizeF)
	{
		for (float y = -height / 2.f; y < height / 2.f; y += raySizeF)
		{
			auto result = shape.CalculatePoint(x, y);
			if (result.first)
			{
				Vector4 camera(0.f, 0.f, 10.f, 0.f);

				Vector4 v = (camera - Vector4(x, y, result.second, 0.f)).Normalize();
				Vector4 gradient = shape.CalculateGradient(x, y, result.second).Normalize();

				float intensity = std::powf(std::max(v * gradient, 0.f), static_cast<float>(UserInterface::values.lightIntensity)); 
			
				for (float i = -raySizeF / 2.f; i < raySizeF / 2.f; i += rayStep)
				{
					for (float j = -raySizeF / 2.f; j < raySizeF / 2.f; j += rayStep)
					{
						points.push_back(RayCasterPoint{ 
							.point{ (x + i) / width * 2.f, (y + j) / height * 2.f, 0.f, 1.f }, 
							.color{ intensity, intensity, intensity, 1.0f } 
						});
					}
				}
			}
		}
	}

	Save();
}

void RayCaster::Save()
{
	GLCall(glBindVertexArray(VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(RayCasterPoint), points.data(), GL_STATIC_DRAW));
	GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(RayCasterPoint), (void*)offsetof(RayCasterPoint, point)));
	GLCall(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RayCasterPoint), (void*)offsetof(RayCasterPoint, color)));
	
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glBindVertexArray(0));
}

void RayCaster::Render()
{
	shader.Bind();

	GLCall(glBindVertexArray(VAO));
	GLCall(glDrawArrays(GL_POINTS, 0, points.size()));
	GLCall(glBindVertexArray(0));

	shader.Unbind();
}
