#pragma once

#include <vector>
#include "Objects/Objects.h"

class ShapeManager
{
private:
    static ShapeManager* instance;
    std::vector<Shape> shapes;

    ShapeManager();
    ~ShapeManager();
public:
    ShapeManager(const ShapeManager&) = delete;
    ShapeManager& operator=(const ShapeManager&) = delete;

    static ShapeManager* GetInstance();
};
