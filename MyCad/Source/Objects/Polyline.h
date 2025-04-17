#pragma once

#include <memory>

#include "Shape.h"
#include "Point.h"
#include "Engine/Renderer.h"

class Polyline : public std::enable_shared_from_this<Polyline>, public Shape, public IObserver
{
private:
	Renderer<PositionVertexData> renderer;

	std::vector<std::weak_ptr<Point>> points;
public:
	Polyline();

	void Render() override;
	void UpdatePoints();

	void AddPoint(std::shared_ptr<Point> point);

	void OnNotified() override;
};