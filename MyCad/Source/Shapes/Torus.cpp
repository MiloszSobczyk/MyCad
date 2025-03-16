#include "Torus.h"
#include <numbers>

Torus::Torus(float majorR, float minorR, int majorDivisions, int minorDivisions)
	: majorR(majorR), minorR(minorR), majorDivisions(majorDivisions), minorDivisions(minorDivisions), 
		center(0.f, 0.f, 0.f), vertices(), indices()
{
}

void Torus::CalculatePoints()
{
	for (int i = 0; i < majorDivisions; ++i)
	{
		float u = 2.f * std::numbers::pi_v<float> * i / static_cast<float>(majorDivisions);

		for (int j = 0; j < minorDivisions; ++j)
		{
			float v = 2.f * std::numbers::pi_v<float> * j / static_cast<float>(minorDivisions);

			float x = (majorR + minorR * sinf(v)) * cosf(u);
			float y = (majorR + minorR * sinf(v)) * sinf(u);
			float z = minorR * cosf(v);

			vertices.push_back(PositionVertexData{ .Position = Vector4(x, y, z) });
		}
	}

	for (int i = 0; i < majorDivisions; ++i)
	{
		int nextI = (i + 1) % majorDivisions;

		for (int j = 0; j < minorDivisions; ++j)
		{
			int nextJ = (j + 1) % minorDivisions;

			indices.push_back(i * minorDivisions + j);
			indices.push_back(i * minorDivisions + nextJ);
			indices.push_back(nextI * minorDivisions + j);
		}
	}
}
