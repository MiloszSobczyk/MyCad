#include "ShapeCreator.h"
#include "Point.h"
#include "Torus.h"
#include "Polyline.h"

std::shared_ptr<Shape> ShapeCreator::CreateShape(ShapeEnum shape) const
{
    switch (shape)
    {
    case ShapeEnum::Point:
        return std::make_shared<Point>();
    case ShapeEnum::Torus:
        return std::make_shared<Torus>();
    }
    throw std::runtime_error("Invalid shape");
}

const std::vector<std::pair<ShapeEnum, std::string>>& ShapeCreator::GetShapeList()
{
    static const std::vector<std::pair<ShapeEnum, std::string>> shapeList = {
        { ShapeEnum::Point, "Point" },
        { ShapeEnum::Torus, "Torus" },
        { ShapeEnum::Polyline, "Polyline" },
        { ShapeEnum::BezierCurve, "Bezier Curve" }
    };
    return shapeList;
}
