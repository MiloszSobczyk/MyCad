#include "BezierSurfaceC0.h"

BezierSurfaceC0::BezierSurfaceC0(Algebra::Vector4 position, bool isCylinder, float width, float height, 
	int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), widthPatches(widthPatches), 
	heightPatches(heightPatches), isCylinder(isCylinder)
{
	const int columns = widthPatches * 3 + 1;
	const int rows = heightPatches * 3 + 1;

	float dx = width / static_cast<float>(widthPatches * 3);
	float dy = height / static_cast<float>(heightPatches * 3);

	Algebra::Vector4 startingPosition = position - Algebra::Vector4(width / 2.f, height / 2.f, 0.f);

	controlPoints.reserve(rows * columns);

	// Width is along X axis and height is along Y axis, Z axis is flat
	// storing points by columns then rows, like in matrix
	// 0 1 2 3 4
	// 5 6 7 8 9

	for (int i = 0; i < columns; ++i)
	{
		for (int j = 0; j < rows; ++j)
		{
			auto widthOffset = Algebra::Vector4(i * dx, 0.f, 0.f);
			auto heightOffset = Algebra::Vector4(0.f, j * dy, 0.f);

			auto pos = startingPosition + widthOffset + heightOffset;

			auto point = std::make_shared<Point>();
			point->GetTranslationComponent()->SetTranslation(startingPosition + widthOffset + heightOffset);

			controlPoints.push_back(point);
		}
	}

	UpdateSurface();
}

void BezierSurfaceC0::OnNotified()
{
}

void BezierSurfaceC0::Render()
{
	for (auto point : controlPoints)
	{
		point->Render();
	}
}

void BezierSurfaceC0::UpdateSurface()
{
}
