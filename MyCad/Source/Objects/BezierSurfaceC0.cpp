#include "BezierSurfaceC0.h"

// ADD REMOVING SINGULAR PATCHES

BezierSurfaceC0::BezierSurfaceC0(Algebra::Vector4 position, bool isCylinder, float width, float height, 
	int widthPatches, int heightPatches)
	: renderer(VertexDataType::PositionVertexData), widthPatches(widthPatches), 
	heightPatches(heightPatches), isCylinder(isCylinder)
{
	name = "BezierSurfaceC0_" + std::to_string(id);
	
	const int columns = widthPatches * 3 + 1;
	const int rows = heightPatches * 3 + 1;

	float dx = width / static_cast<float>(widthPatches * 3);
	float dy = height / static_cast<float>(heightPatches * 3);

	Algebra::Vector4 startingPosition = position - Algebra::Vector4(width / 2.f, height / 2.f, 0.f);

	controlPoints.reserve(rows * columns);


	// Width is along X axis and height is along Y axis, Z axis is flat
	// storing points by columns then rows, like in matrix
	// 0 1 2 3
	// 4 5 6 7

	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < columns; ++j)
		{
			auto heightOffset = Algebra::Vector4(0.f, i * dy, 0.f);
			auto widthOffset = Algebra::Vector4(j * dx, 0.f, 0.f);

			auto pos = startingPosition + widthOffset + heightOffset;

			auto point = std::make_shared<Point>("SurfacePoint_");
			point->GetTranslationComponent()->SetTranslation(startingPosition + widthOffset + heightOffset);

			controlPoints.push_back(point);
		}
	}

	for (int patchIndex = 0; patchIndex < widthPatches * heightPatches; ++patchIndex)
	{
		std::vector<std::weak_ptr<Point>> points;
		std::vector<std::size_t> indices;

		int startingI = patchIndex / widthPatches;
		int startingJ = patchIndex % widthPatches;

		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				points.push_back(controlPoints[startingI + i * columns + startingJ + j]);
			}
		}

		indices.push_back(5);
		indices.push_back(6);
		indices.push_back(9);
		indices.push_back(10);

		if (startingI == 0)
		{
			if (startingJ == 0)
			{
				indices.push_back(0);
			}
			else if (startingJ == widthPatches - 1)
			{
				indices.push_back(3);
			}
			indices.push_back(1);
			indices.push_back(2);
		}
		else if (startingI == heightPatches - 1)
		{
			if (startingJ == 0)
			{
				indices.push_back(12);
			}
			else if (startingJ == widthPatches - 1)
			{
				indices.push_back(15);
			}
			indices.push_back(13);
			indices.push_back(14);
		}

		if (startingJ == 0)
		{
			indices.push_back(4);
			indices.push_back(8);
		}
		else if (startingJ == widthPatches - 1)
		{
			indices.push_back(7);
			indices.push_back(11);
		}

		std::sort(indices.begin(), indices.end());
		patches.push_back(Patch(points, indices));
	}

	UpdateSurface();
}

void BezierSurfaceC0::OnNotified()
{
}

void BezierSurfaceC0::Init()
{
	for (auto point : controlPoints)
	{
		point->Lock(shared_from_this());
	}
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
