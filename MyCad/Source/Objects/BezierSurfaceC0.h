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

	void Render() override;
	void UpdateSurface();

public:
	BezierSurfaceC0(Algebra::Vector4 position, bool isCylinder = false, float width = 1.f, float height = 1.f, 
		int widthPatches = 1, int heightPatches = 1);

	inline std::vector<std::shared_ptr<Point>> GetControlPoints() { return controlPoints; };

	void OnNotified() override;
	void Init();
	void RenderUI() override;
};