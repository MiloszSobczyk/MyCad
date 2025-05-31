#pragma once

#include <memory>

#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Shape.h"
#include "Patch.h"
#include "Polyline.h"
#include "ConnectionType.h"

class BezierSurfaceC0 : public std::enable_shared_from_this<BezierSurfaceC0>, public Shape, public IObserver
{
private:
	Renderer<PositionVertexData> renderer;

	std::vector<std::shared_ptr<Point>> controlPoints;
	std::vector<Patch> patches;
	std::shared_ptr<Polyline> bernsteinPolygon;

	int widthPatches = 0;
	int heightPatches = 0;
	ConnectionType connectionType = ConnectionType::Flat;
	bool isCylinder = false;

	int tessLevelU = 4;
	int tessLevelV = 4;
	bool drawBernsteinPolygon = false;

	bool somethingChanged = false;

private:
	inline int GetColumns() const { return widthPatches * 3 + 1; };
	inline int GetRows() const { return heightPatches * 3 + 1; };
	std::shared_ptr<Point> GetPointAt(int row, int col) const;
	
	inline void OnNotified() override { somethingChanged = true; };

	void SetupControlPoints(Algebra::Vector4 position, float width, float height);
	void SetupPatches();

	void Render() override;
	void Update();

	// TODO: Move to patch. Patch should have both Bernstein and DeBoor's polygon 
	void SetupPolygon();

	int HasDuplicates(const json& controlPointsJson);
	std::vector<unsigned int> ProcessPoints(std::vector<unsigned int> ids, int connectionType);

public:
	// TODO: Change to GetUniqueControlPoints
	inline std::vector<std::shared_ptr<Point>> GetControlPoints() { return controlPoints; };

	BezierSurfaceC0();
	void InitNormally(std::vector<std::shared_ptr<Point>>& jsonPoints);
	void InitAsCylinder(std::vector<std::shared_ptr<Point>>& jsonPoints);

	BezierSurfaceC0(ConnectionType connectionType, Algebra::Vector4 position, float width, float height, int widthPatches, int heightPatches);

	void Init();
	
	void RenderUI() override;

	json Serialize() const override;
	static std::shared_ptr<BezierSurfaceC0> Deserialize(const json& j);
};