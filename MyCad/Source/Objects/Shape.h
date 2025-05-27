#pragma once

#include <string>
#include <imgui/imgui.h>
#include <nlohmann/json.hpp>

#include "Algebra.h"
#include "Components/Transformations.h"
#include "Utils/ColorPalette.h"
#include "Utils/Serializer.h"

using json = nlohmann::json;

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

	inline int GetId() const { return id; }

    virtual Algebra::Matrix4 GetModelMatrix() const;
    virtual void RenderUI();

	virtual json Serialize() const = 0;
    virtual void Deserialize(const json& j) = 0;
};