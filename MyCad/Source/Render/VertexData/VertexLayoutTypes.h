#pragma once

#include "Algebra.h"
#include "VertexLayout.h"

struct PositionVertex 
{
    Algebra::Vector4 position;
};

struct PositionNormalVertex 
{
    Algebra::Vector4 position;
    Algebra::Vector4 normal;
};


class PositionVertexLayout : public VertexLayout
{
    void SetupVAO() const override;
};

class PositionNormalVertexLayout : public VertexLayout
{
    void SetupVAO() const override;
};

inline void RegisterVertexLayouts() 
{
    VertexLayoutRegistry::Register(typeid(PositionVertex), std::make_shared<PositionVertexLayout>());
    VertexLayoutRegistry::Register(typeid(PositionNormalVertex), std::make_shared<PositionNormalVertexLayout>());
}