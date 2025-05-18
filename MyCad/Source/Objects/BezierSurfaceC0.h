#pragma once

#include <memory>

#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Shape.h"
#include "Patch.h"


class BezierSurfaceC0 : public std::enable_shared_from_this<BezierSurfaceC0>, public Shape, public IObserver
{
private:
	Renderer<PositionVertexData> renderer;
	std::vector<std::shared_ptr<Point>> controlPoints;
	std::vector<Patch> patches;
	std::vector<int> selectedPatches;

	int widthPatches;
	int heightPatches;
	bool isCylinder;

	int tessLevelU = 4;
	int tessLevelV = 4;

	void Render() override;
	void UpdateSurface();

	void RemovePatch(int index);

public:
	BezierSurfaceC0(Algebra::Vector4 position, float width, float height, int widthPatches, int heightPatches);
	// Axes: 0 - X, 1 - Y, 2 - Z
	BezierSurfaceC0(Algebra::Vector4 position, int axis, float radius, float height, int widthPatches, int heightPatches);

	inline std::vector<std::shared_ptr<Point>> GetControlPoints() { return controlPoints; };

	void OnNotified() override;
	void Init();
	void RenderUI() override;
};