#include "ShapeManager.h"

ShapeManager* ShapeManager::instance = nullptr;

ShapeManager::ShapeManager() : shapes()
{
}

ShapeManager::~ShapeManager()
{
    delete instance;
}

ShapeManager* ShapeManager::GetInstance()
{
    if (instance == nullptr)
    {
        instance = new ShapeManager();
    }
    return instance;
}
