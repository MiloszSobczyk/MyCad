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
	void RenderUI() override;
	void UpdatePoints();

	void AddPoint(const std::shared_ptr<Point>& point);
	void RemovePoint(const std::shared_ptr<Point>& point);
	void SwapPoints(int index1, int index2);

	void OnNotified() override;
};