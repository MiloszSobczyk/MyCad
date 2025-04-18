#pragma once

#include "Shape.h"
#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Polyline.h"

#include <vector>
#include <memory>

class BezierCurve : public std::enable_shared_from_this<BezierCurve>, public Shape, public IObserver
{
private:
    bool drawPolyline = false;

    Renderer<PositionVertexData> renderer;
    std::vector<std::weak_ptr<Point>> controlPoints;
    std::shared_ptr<Polyline> polyline;

    void UpdateCurve();
public:
    BezierCurve();

    void Render() override;
    void RenderUI() override;

    void AddPoint(const std::shared_ptr<Point>& point);
    void RemovePoint(const std::shared_ptr<Point>& point);
    void SwapPoints(int index1, int index2);

    void OnNotified() override;
};
