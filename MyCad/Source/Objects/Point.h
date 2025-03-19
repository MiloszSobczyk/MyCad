#pragma once

#include "Shape.h"
#include "Engine/Renderer.h"

class Point : public Shape
{
private:
	Renderer<PositionVertexData> renderer;

    static std::vector<PositionVertexData> vertices;
	static std::vector<unsigned int> indices;

public:
    Point();

	void HandleInput() override;
	void Render() override;
};