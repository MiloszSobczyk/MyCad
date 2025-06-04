#pragma once

#include <memory>

#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Shape.h"
#include "Patch.h"
#include "Polyline.h"
#include "ConnectionType.h"

class BezierSurfaceC2 : public std::enable_shared_from_this<BezierSurfaceC2>, public Shape, public IObserver
{
private:
	Renderer<PositionVertexData> renderer;

	std::vector<std::shared_ptr<Point>> controlPoints;
	std::vector<std::shared_ptr<Point>> bernsteinPoints;
	std::vector<Patch> patches;

	int widthPatches = 0;
	int heightPatches = 0;
	ConnectionType connectionType = ConnectionType::Flat;

	int tessLevelU = 4;
	int tessLevelV = 4;
	bool drawBernsteinPolygon = false;
	bool drawDeBoorPolygon = false;
	
	bool somethingChanged = false;

private:
	inline int GetColumns() const { return widthPatches + 3; };
	inline int GetRows() const { return heightPatches + 3; };

	inline void OnNotified() override { somethingChanged = true; };

	void DetectConnectionType();

	void SetupControlPoints(Algebra::Vector4 position, float width, float height);
	void SetupPatches();
	
	void Render() override;
	void Update();

public:
	std::vector<std::shared_ptr<Point>> GetUniqueControlPoints();

	BezierSurfaceC2();
	BezierSurfaceC2(ConnectionType connectionType, Algebra::Vector4 position, float width, float height, int widthPatches, int heightPatches);
	
	void SwapPoints(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint) override;

	void Init();
	void RenderUI() override;

	json Serialize() const override;
	static std::shared_ptr<BezierSurfaceC2> Deserialize(const json& j);
};