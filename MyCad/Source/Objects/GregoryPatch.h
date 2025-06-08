#pragma once

#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Shape.h"

#include <array>
#include <memory>

class Point;

class GregoryPatch : std::enable_shared_from_this<GregoryPatch>, public Shape, public IObserver
{
private:
	Renderer<PositionVertexData> renderer;

	std::array<std::shared_ptr<Point>, 4> edge1;
	std::array<std::shared_ptr<Point>, 4> edge2;
	std::array<std::shared_ptr<Point>, 4> edge3;

	int tessLevelU = 4;
	int tessLevelV = 4;

	bool somethingChanged = false;

private:
	inline void OnNotified() override { somethingChanged = true; };

	void Render() override;
	void Update();

public:
	GregoryPatch(std::array<std::shared_ptr<Point>, 4> edge1, std::array<std::shared_ptr<Point>, 4> edge2,
		std::array<std::shared_ptr<Point>, 4> edge3);

	void ReplacePoint(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint) override;

	void Init() {};

	void RenderUI() override;
};