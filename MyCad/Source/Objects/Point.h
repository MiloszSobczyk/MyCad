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

	void Render() override;
	void RotateAroundPoint(Algebra::Vector4 point, Algebra::Quaternion q) override;
};