#pragma once

#include "Shape.h"
#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Polyline.h"

#include <vector>
#include <memory>


enum class AxisMode
{
	X,
	Y,
	Z
};

class BezierCurveC2 : public std::enable_shared_from_this<BezierCurveC2>, public Shape, public IObserver
{
private:
    bool drawDeBoorPoints = false;
    bool drawBernsteinBase = false;
    bool drawBernsteinPolygon = false;

    Renderer<PositionVertexData> renderer;
    std::vector<std::weak_ptr<Point>> controlPoints;
    std::shared_ptr<Polyline> polyline;
    std::shared_ptr<Polyline> bernsteinPolyline;
    std::vector<std::shared_ptr<Point>> bernsteinPoints;
    int selectedBernsteinIndex = -1;
    AxisMode axisMode;

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
