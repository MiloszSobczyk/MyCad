#pragma once
#include "BezierCurve.h"


class BezierCurveC2 : public BezierCurve
{
protected:
    void UpdateCurve() override;

public:
    BezierCurveC2();

    void Render() override;
    void RenderUI() override;
};