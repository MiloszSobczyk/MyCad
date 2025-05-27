#pragma once

#include <memory>

#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Shape.h"
#include "Patch.h"
#include "Polyline.h"
#include "Point.h"

class BezierSurfaceC2 : public std::enable_shared_from_this<BezierSurfaceC2>, public Shape, public IObserver
{
private:
	Renderer<PositionVertexData> renderer;
	std::vector<std::shared_ptr<Point>> controlPoints;
	std::vector<Patch> patches;
	std::vector<int> selectedPatches;
	std::vector<std::shared_ptr<Point>> bernsteinPoints;
	std::shared_ptr<Polyline> bernsteinPolygon;
	std::shared_ptr<Polyline> deBoorPolygon;

	int widthPatches;
	int heightPatches;
	bool isCylinder;

	int tessLevelU = 4;
	int tessLevelV = 4;
	bool drawBernsteinPolygon = false;
	bool drawDeBoorPolygon = false;


	void Render() override;
	void UpdateSurface();

public:
	BezierSurfaceC2(Algebra::Vector4 position, float width, float height, int widthPatches, int heightPatches);
	// Axes: 0 - X, 1 - Y, 2 - Z
	BezierSurfaceC2(Algebra::Vector4 position, int axis, float radius, float height, int widthPatches, int heightPatches);

	inline std::vector<std::shared_ptr<Point>> GetControlPoints() { return controlPoints; };

	void OnNotified() override;
	void Init();
	void RenderUI() override;

	std::shared_ptr<Point> GetPointAt(int row, int col) const;
	std::shared_ptr<Point> GetBernsteinPointAt(int row, int col);
	void SetupPolygon();
	void SetupDeBoorPolygon();

	void UpdateColors();
	inline void ClearSelection() { selectedPatches.clear(); UpdateColors(); }

	json Serialize() const override;
};