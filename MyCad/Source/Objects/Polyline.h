#pragma once

#include <memory>

#include "Shape.h"
#include "Point.h"
#include "Engine/Renderer.h"

class Polyline : public Shape
{
private:
	Renderer<PositionVertexData> renderer;

	std::vector<std::weak_ptr<Point>> points;
public:
	Polyline();

	void HandleInput() override;
	void Render() override;

	void AddPoint(std::shared_ptr<Point> point);
};