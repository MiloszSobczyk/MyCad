#include "IdManager.h"
#include "Objects/Shape.h"
#include <algorithm>

unsigned int IdManager::currentId = 0;
std::vector<std::weak_ptr<Shape>> IdManager::shapes;

unsigned int IdManager::GetNextId()
{
    return ++currentId;
}

void IdManager::RegisterShape(const std::shared_ptr<Shape>& shape)
{
    for (auto it = shapes.begin(); it != shapes.end(); ) {
        if (it->expired()) 
        {
            it = shapes.erase(it);
        }
        else if (it->lock() == shape) 
        {
            return;
        }
        else {
            ++it;
        }
    }

    shapes.push_back(shape);
}

void IdManager::UnregisterShape(const std::shared_ptr<Shape>& shape)
{
    shapes.erase(
        std::remove_if(
            shapes.begin(), shapes.end(),
            [&](const std::weak_ptr<Shape>& wptr) {
                auto sp = wptr.lock();
                return !sp || sp == shape;
            }
        ),
        shapes.end()
    );
}

std::vector<std::shared_ptr<Shape>> IdManager::GetShapes()
{
    std::vector<std::shared_ptr<Shape>> result;
    result.reserve(shapes.size());
    for (auto& wptr : shapes)
    {
        if (auto sp = wptr.lock()) 
        {
            result.push_back(sp);
        }
    }

    return result;
}

std::shared_ptr<Shape> IdManager::GetById(unsigned int id)
{
    for (auto it = shapes.begin(); it != shapes.end(); )
    {
        if (auto sp = it->lock()) 
        {
            if (sp->GetId() == id)
            {
                return sp;
            }
            ++it;
        }
        else 
        {
            it = shapes.erase(it);
        }
    }

    return nullptr;
}
