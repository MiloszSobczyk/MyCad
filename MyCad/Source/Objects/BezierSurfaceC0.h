#pragma once

#include <memory>

#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Shape.h"
#include "Patch.h"
#include "Polyline.h"

class BezierSurfaceC0 : public std::enable_shared_from_this<BezierSurfaceC0>, public Shape, public IObserver
{
private:
	Renderer<PositionVertexData> renderer;
	std::vector<std::shared_ptr<Point>> controlPoints;
	std::vector<Patch> patches;
	std::vector<int> selectedPatches;
	std::shared_ptr<Polyline> bernsteinPolygon;
	bool somethingChanged = false;

	int widthPatches;
	int heightPatches;
	bool isCylinder;

	int tessLevelU = 4;
	int tessLevelV = 4;
	bool drawBernsteinPolygon = false;

	void Render() override;
	void UpdateSurface();
	std::shared_ptr<Point> GetPointAt(int row, int col) const;

	int HasDuplicates(const json& controlPointsJson);
	std::vector<unsigned int> ProcessPoints(std::vector<unsigned int> ids, int connectionType);

public:
	BezierSurfaceC0();
	void InitNormally(std::vector<std::shared_ptr<Point>>& jsonPoints);
	void InitAsCylinder(std::vector<std::shared_ptr<Point>>& jsonPoints);

	BezierSurfaceC0(Algebra::Vector4 position, float width, float height, int widthPatches, int heightPatches);
	// Axes: 0 - X, 1 - Y, 2 - Z
	BezierSurfaceC0(Algebra::Vector4 position, int axis, float radius, float height, int widthPatches, int heightPatches);
	void SetupPolygon();

	void OnNotified() override;
	void Init();
	void UpdateColors();
	
	void RenderUI() override;
	
	inline std::vector<std::shared_ptr<Point>> GetControlPoints() { return controlPoints; };
	inline void ClearSelection() { selectedPatches.clear(); UpdateColors(); }

	json Serialize() const override;
	static std::shared_ptr<BezierSurfaceC0> Deserialize(const json& j);
};