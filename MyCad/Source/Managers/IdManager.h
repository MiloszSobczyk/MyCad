#pragma once

#include <vector>
#include <memory>

class Shape;

class IdManager
{
private:
    static unsigned int currentId;
    static std::vector<std::weak_ptr<Shape>> shapes;

public:
    static unsigned int GetNextId();
    static std::vector<std::shared_ptr<Shape>> GetShapes();
    static std::shared_ptr<Shape> GetById(unsigned int id);

    static void RegisterShape(const std::shared_ptr<Shape>& shape);
    static void UnregisterShape(const std::shared_ptr<Shape>& shape);
};