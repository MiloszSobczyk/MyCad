#include "InfiniteGrid.h"
#include "Managers/ShaderManager.h"

const std::vector<PositionVertexData> squareData = {
	PositionVertexData{{ -1.f,  0.f, -1.f,  1.f }},
	PositionVertexData{{  1.f,  0.f, -1.f,  1.f }},
	PositionVertexData{{  1.f,  0.f,  1.f,  1.f }},
	PositionVertexData{{ -1.f,  0.f,  1.f , 1.f }}
};

const std::vector<unsigned int> squareIndices = {
	0, 2, 1,
	2, 0, 3
};

InfiniteGrid::InfiniteGrid()
	: renderer(VertexDataType::PositionVertexData, squareData, squareIndices)
{
	renderer.SetIndices(squareIndices);
	renderer.SetVertices(squareData);
}

void InfiniteGrid::Render(Algebra::Matrix4 viewMatrix, Algebra::Matrix4 projectionMatrix, Algebra::Vector4 cameraPosition)
{
	auto shader = ShaderManager::GetInstance().GetShader(ShaderName::InfiniteGrid);
	shader->Bind();
	shader->SetUniformMat4f("u_viewMatrix", viewMatrix);
	shader->SetUniformMat4f("u_projectionMatrix", projectionMatrix);
	shader->SetUniformVec4f("u_CameraWorldPosition", cameraPosition);
	renderer.Render(GL_TRIANGLES);
	shader->Unbind();
}
