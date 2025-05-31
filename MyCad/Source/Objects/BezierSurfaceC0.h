#pragma once

#include <memory>

#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Shape.h"
#include "Patch.h"
#include "Polyline.h"
#include "ConnectionType.h"

// TODO: add uniform manager

class BezierSurfaceC0 : public std::enable_shared_from_this<BezierSurfaceC0>, public Shape, public IObserver
{
private:
	Renderer<PositionVertexData> renderer;

	std::vector<std::shared_ptr<Point>> controlPoints;
	std::vector<Patch> patches;

	int widthPatches = 0;
	int heightPatches = 0;
	ConnectionType connectionType = ConnectionType::Flat;

	int tessLevelU = 4;
	int tessLevelV = 4;
	bool drawBernsteinPolygon = false;

	bool somethingChanged = false;

private:
	inline int GetColumns() const { return widthPatches * 3 + 1; };
	inline int GetRows() const { return heightPatches * 3 + 1; };
	
	inline void OnNotified() override { somethingChanged = true; };

	void SetupControlPoints(Algebra::Vector4 position, float width, float height);
	void SetupPatches();

	void Render() override;
	void Update();
public:
	std::vector<std::shared_ptr<Point>> GetUniqueControlPoints();

	BezierSurfaceC0();
	BezierSurfaceC0(ConnectionType connectionType, Algebra::Vector4 position, float width, float height, int widthPatches, int heightPatches);
	
	void Init();
	
	void RenderUI() override;

	json Serialize() const override;
	static std::shared_ptr<BezierSurfaceC0> Deserialize(const json& j);
};