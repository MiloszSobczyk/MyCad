#pragma once

#include <memory>
#include <vector>

#include "Shape.h"
#include "Point.h"

class Polyline : public std::enable_shared_from_this<Polyline>, public Shape, public IObserver
{
private:
    std::vector<std::weak_ptr<Point>> points;

    Renderer<PositionVertexData> renderer;

public:
    Polyline();
    Polyline(std::vector<std::weak_ptr<Point>> points);

    void Render() override;
    void RenderUI() override;
    void UpdatePoints();

    void AddPoint(const std::shared_ptr<Point>& point);
    void RemovePoint(const std::shared_ptr<Point>& point);
    void SwapPoints(int index1, int index2);
    void ClearPoints();

    void OnNotified() override;

    void InitFromPoints();

    json Serialize() const override;
};
