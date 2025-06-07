#pragma once

#include <string>
#include <imgui/imgui.h>

#include "Algebra.h"
#include "Components/Transformations.h"
#include "Utils/ColorPalette.h"
#include "Utils/Serializer.h"

class Point;

// TODO: Change RemovePoint from collections so it takes in index instead of a pointer to the point
// TODO: Add ClearAdjacentDuplicates method that removes duplicate points if they are adjacent to each other

class Shape
{
private:
    static int idCounter;

protected:
    int id;
    std::string name;
    Algebra::Vector4 color;

    // TODO: make us of those lines in the future, replacing all the UpdateSurface methods and stuff
    //bool somethingChanged = false;
    virtual void Update();

public:
    Shape();
    virtual ~Shape() = default;

    virtual void Render() = 0;

    inline Algebra::Vector4 GetColor() const { return color; }
    inline void SetColor(Algebra::Vector4 color) { this->color = color; }

    inline std::string GetName() const { return name; }
    inline void SetName(std::string name) { this->name = std::move(name); }

	inline int GetId() const { return id; }

    virtual Algebra::Matrix4 GetModelMatrix() const;
    virtual void RenderUI();

	virtual json Serialize() const = 0;

    virtual void ReplacePoint(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint) {};

    inline static void ResetIdCounter() { idCounter = 0; }
};