#pragma once

#include <string>
#include <imgui/imgui.h>

#include "Algebra.h"
#include "Components/Transformations.h"

class Shape
{
private:
    static int idCounter;

protected:
    int id;
    std::string name;
    Algebra::Vector4 color;

public:
    Shape();
    virtual ~Shape() = default;

    virtual void Render() = 0;

    inline Algebra::Vector4 GetColor() const { return color; }
    inline void SetColor(Algebra::Vector4 color) { this->color = color; }

    inline std::string GetName() const { return name; }
    inline void SetName(std::string name) { this->name = std::move(name); }

    virtual Algebra::Matrix4 GetModelMatrix() const;
    virtual void RenderUI();

    //TODO: remove and move whole logic to operations code
	virtual void RotateAroundPoint(Algebra::Vector4 point, Algebra::Quaternion q);
	void ScaleAroundPoint(Algebra::Vector4 point, Algebra::Vector4 scaleFactor);
};