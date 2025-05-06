#pragma once

#include "Shape.h"
#include "Core/IObserver.h"
#include "Engine/Renderer.h"
#include "Point.h"
#include "Polyline.h"

#include <vector>
#include <memory>



class InterpolatingCurve : public std::enable_shared_from_this<InterpolatingCurve>, public Shape, public IObserver
{
enum class AxisMode
{
    X = 0,
    Y = 1,
    Z = 2,
};

private:
    bool drawBernsteinPolygon = false;
    bool drawInterpolatingPolygon = false;

    Renderer<PositionVertexData> renderer;
    std::vector<std::weak_ptr<Point>> controlPoints;
    std::shared_ptr<Polyline> bernsteinPolyline;
    std::shared_ptr<Polyline> interpolatingPolyline;

protected:
    virtual void UpdateCurve();

public:
    InterpolatingCurve();

    virtual void Render() override;
    virtual void RenderUI() override;

    void AddPoint(const std::shared_ptr<Point>& point);
    void RemovePoint(const std::shared_ptr<Point>& point);
    void SwapPoints(int index1, int index2);

    void OnNotified() override;

    std::vector<Algebra::Vector4> SolveTrilinearMatrix(std::vector<float>& alpha, 
        std::vector<float>& beta, std::vector<Algebra::Vector4>& r);
};
