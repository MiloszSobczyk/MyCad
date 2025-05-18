#pragma once

#include "Shape.h"
#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Polyline.h"

#include <vector>
#include <memory>

class BezierCurveC2 : public std::enable_shared_from_this<BezierCurveC2>, public Shape, public IObserver
{
enum class AxisMode
{
	X = 0,
	Y = 1,
	Z = 2,
};

private:
    bool drawDeBoorPolygon = false;
    bool drawBernsteinBase = false;
    bool drawBernsteinPolygon = false;

    std::vector<std::shared_ptr<Point>> bernsteinPoints;
    Renderer<PositionVertexData> renderer;
    std::vector<std::weak_ptr<Point>> controlPoints;
    std::shared_ptr<Polyline> polyline;
    std::shared_ptr<Polyline> bernsteinPolyline;
    int selectedBernsteinIndex = -1;
    AxisMode axisMode;
    Algebra::Vector4 lastTranslation;

protected:
    virtual void UpdateCurve();

public:
    BezierCurveC2();

    virtual void Render() override;
    virtual void RenderUI() override;

    void AddPoint(const std::shared_ptr<Point>& point);
    void RemovePoint(const std::shared_ptr<Point>& point);
    void SwapPoints(int index1, int index2);

    void OnNotified() override;
    void HandleInput();
    void RecalculateDeBoorPoints();
};
