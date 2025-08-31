#pragma once

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

#include <iostream>
#include <numbers>

namespace ShapeCreator
{
	Entity CreatePoint(Ref<Scene> scene);
}

namespace Curves
{
	namespace C0
	{
		inline void PadBezierVertices(std::vector<Algebra::Vector4>& vertices)
		{
			int rest = static_cast<int>(vertices.size() % 4);
			if (rest == 0) return;

			switch (rest)
			{
			case 1:
			{
				auto p = vertices.back();
				vertices.insert(vertices.end(), 3, p);
				break;
			}
			case 2:
			{
				auto p0 = vertices[vertices.size() - 2];
				auto p1 = vertices.back();
				vertices.pop_back();
				vertices.pop_back();
				vertices.push_back(p0);
				vertices.push_back(p0);
				vertices.push_back(p1);
				vertices.push_back(p1);
				break;
			}
			case 3:
			{
				auto p0 = vertices[vertices.size() - 3];
				auto p1 = vertices[vertices.size() - 2];
				auto p2 = vertices.back();
				vertices.pop_back();
				vertices.pop_back();
				vertices.pop_back();
				vertices.push_back(p0);
				vertices.push_back((1.f / 3.f) * p0 + (2.f / 3.f) * p1);
				vertices.push_back((2.f / 3.f) * p1 + (1.f / 3.f) * p2);
				vertices.push_back(p2);
				break;
			}
			}
		}

		inline std::vector<Algebra::Vector4> UpdateCurve(CurveComponent& curve, Ref<Scene> scene)
		{
			auto& bernsteinPolyline = Entity{ curve.bernsteinPolylineHandle, scene.get() }
			.GetComponent<PolylineComponent>();

			std::vector<Algebra::Vector4> result;

			int i = 0;
			for (auto pointHandle : bernsteinPolyline.pointHandles)
			{
				Entity pointEntity{ pointHandle, scene.get() };

				if (pointEntity.HasComponent<TranslationComponent>())
				{
					Algebra::Vector4 vertex = pointEntity.GetComponent<TranslationComponent>().translation;
					vertex.w = 1.0f;
					result.push_back(vertex);
					++i;

					if (i % 4 == 0)
					{
						// duplicate every 4th point
						result.push_back(vertex);
						++i;
					}
				}
			}

			PadBezierVertices(result);

			return result;
		}
	}
	namespace C2
	{
		inline std::vector<Algebra::Vector4> UpdateCurve(CurveComponent& curveComponent, Ref<Scene> scene)
		{
			auto& controlPolyline = Entity{ curveComponent.controlPolylineHandle, scene.get() }
				.GetComponent<PolylineComponent>();

			auto& pointHandles = controlPolyline.pointHandles;

			std::vector<Algebra::Vector4> vertices;
			Entity bernsteinPolyline{ curveComponent.bernsteinPolylineHandle, scene.get() };

			for (size_t i = 0; i + 3 < pointHandles.size(); ++i)
			{
				auto d0 = Entity{ pointHandles[i]    , scene.get() };
				auto d1 = Entity{ pointHandles[i + 1], scene.get() };
				auto d2 = Entity{ pointHandles[i + 2], scene.get() };
				auto d3 = Entity{ pointHandles[i + 3], scene.get() };

				Algebra::Vector4 D0 = d0.GetComponent<TranslationComponent>().translation;
				Algebra::Vector4 D1 = d1.GetComponent<TranslationComponent>().translation;
				Algebra::Vector4 D2 = d2.GetComponent<TranslationComponent>().translation;
				Algebra::Vector4 D3 = d3.GetComponent<TranslationComponent>().translation;

				D0.w = D1.w = D2.w = D3.w = 1.0f;

				Algebra::Vector4 bezierPoints[4] = {
					(D0 + 4.0f * D1 + D2) / 6.0f,
					(2.0f * D1 + D2) / 3.0f,
					(D1 + 2.0f * D2) / 3.0f,
					(D1 + 4.0f * D2 + D3) / 6.0f
				};

				for (int j = 0; j < 4; ++j)
				{
					auto pointEntity = ShapeCreator::CreatePoint(scene);
					pointEntity.EmplaceTag<IsInvisibleTag>();
					pointEntity.GetComponent<TranslationComponent>().SetTranslation(bezierPoints[j]);
					pointEntity.EmplaceComponent<VirtualComponent>(bernsteinPolyline.GetHandle());
					
					bernsteinPolyline.GetComponent<PolylineComponent>().pointHandles.push_back(pointEntity.GetHandle());

					vertices.push_back(bezierPoints[j]);
				}
			}

			return vertices;
		}
	}
	namespace Interpolating
	{
		inline std::vector<Algebra::Vector4> SolveTrilinearMatrix(std::vector<float>& alpha,
			std::vector<float>& beta, std::vector<Algebra::Vector4>& r)
		{
			const std::size_t m = r.size();
			std::vector<float> gamma(m);
			std::vector<Algebra::Vector4> delta(m), c(m);

			if (r.size() == 1)
			{
				c[0] = r[0] / 2.f;
				c[0].w = 0.0f;
				return c;
			}

			float denom = 2.0f;
			gamma[0] = beta[0] / denom;
			delta[0] = r[0] / denom;
			delta[0].w = 0.0f;

			for (std::size_t i = 1; i < m; ++i)
			{
				denom = 2.0f - alpha[i - 1] * gamma[i - 1];
				if (i < m - 1)
				{
					gamma[i] = beta[i] / denom;
				}
				Algebra::Vector4 tmp = r[i] - alpha[i - 1] * delta[i - 1];
				delta[i] = tmp / denom;
				delta[i].w = 0.0f;
			}

			c[m - 1] = delta[m - 1];
			c[m - 1].w = 0.0f;
			for (int i = static_cast<int>(m) - 2; i >= 0; --i)
			{
				Algebra::Vector4 tmp = delta[i] - gamma[i] * c[i + 1];
				c[i] = tmp;
				c[i].w = 0.0f;
			}

			return c;
		}

		inline std::vector<Algebra::Vector4> CalculateInterpolatedBernsteinPoints(std::vector<Algebra::Vector4>& positions)
		{
			std::vector<float> dists;
			std::vector<float> alpha;
			std::vector<float> beta;
			std::vector<Algebra::Vector4> r;

			for (int i = 0; i < positions.size() - 1; i++)
			{
				dists.push_back((positions[i + 1] - positions[i]).Length());
			}

			for (int i = 1; i < dists.size(); i++)
			{
				float d0 = dists[i - 1];
				float d1 = dists[i];
				if (i != 1)
					alpha.push_back(d0 / (d0 + d1));
				if (i != dists.size() - 1)
					beta.push_back(d1 / (d0 + d1));

				Algebra::Vector4 P0 = (positions[i] - positions[i - 1]) / d0;
				Algebra::Vector4 P1 = (positions[i + 1] - positions[i]) / d1;

				r.push_back(3.f * (P1 - P0) / (d0 + d1));
			}

			auto c = SolveTrilinearMatrix(alpha, beta, r);
			c.insert(c.begin(), Algebra::Vector4());
			c.push_back(Algebra::Vector4());
			std::vector<Algebra::Vector4> a(c.size());
			std::vector<Algebra::Vector4> b(c.size());
			std::vector<Algebra::Vector4> d(c.size());

			for (int i = 0; i < dists.size(); i++)
				d[i] = (c[i + 1] - c[i]) / dists[i] / 3.f;

			for (int i = 0; i < a.size(); i++)
				a[i] = positions[i];

			for (int i = 0; i < b.size() - 1; i++)
				b[i] = (a[i + 1] - a[i]) / dists[i] - c[i] * dists[i] - d[i] * dists[i] * dists[i];

			std::vector<Algebra::Vector4> bernsteinPoints;

			for (int i = 0; i < dists.size(); ++i)
			{
				Algebra::Vector4 ai = a[i];
				Algebra::Vector4 bi = b[i] * dists[i];
				Algebra::Vector4 ci = c[i] * dists[i] * dists[i];
				Algebra::Vector4 di = d[i] * dists[i] * dists[i] * dists[i];

				Algebra::Vector4 P0 = ai;
				Algebra::Vector4 P1 = ai + bi / 3.f;
				Algebra::Vector4 P2 = ai + bi * 2.f / 3.f + ci / 3.f;
				Algebra::Vector4 P3 = ai + bi + ci + di;
				P0.w = P1.w = P2.w = P3.w = 1.0f;

				bernsteinPoints.push_back(P0);
				bernsteinPoints.push_back(P1);
				bernsteinPoints.push_back(P2);
				bernsteinPoints.push_back(P3);
			}

			return bernsteinPoints;
		}

		inline std::vector<Algebra::Vector4> UpdateCurve(CurveComponent& curveComponent, Ref<Scene> scene)
		{
			auto& controlPolyline = Entity{ curveComponent.controlPolylineHandle, scene.get() }
			.GetComponent<PolylineComponent>();

			auto& pointHandles = controlPolyline.pointHandles;

			std::vector<Algebra::Vector4> positions;

			for (auto pointHandle : pointHandles)
			{
				Entity point{ pointHandle, scene.get() };
				positions.push_back(point.GetComponent<TranslationComponent>().translation);
			}

			std::vector<Algebra::Vector4> vertices;

			if (positions.size() <= 1)
			{
				return {};
			}
			else if (positions.size() == 2)
			{
				vertices.push_back(positions[0]);
				vertices.push_back(positions[0]);
				vertices.push_back(positions[1]);
				vertices.push_back(positions[1]);

				vertices[0].w = 1.f;
				vertices[1].w = 1.f;
				vertices[2].w = 1.f;
				vertices[3].w = 1.f;
			}
			else
			{
				vertices = CalculateInterpolatedBernsteinPoints(positions);
			}

			Entity bernsteinPolyline{ curveComponent.bernsteinPolylineHandle, scene.get() };

			for (size_t i = 0; i < vertices.size(); ++i)
			{
				auto pointEntity = ShapeCreator::CreatePoint(scene);
				pointEntity.EmplaceTag<IsInvisibleTag>();
				pointEntity.GetComponent<TranslationComponent>().SetTranslation(vertices[i]);
				pointEntity.EmplaceComponent<VirtualComponent>(bernsteinPolyline.GetHandle());

				bernsteinPolyline.GetComponent<PolylineComponent>().pointHandles.push_back(pointEntity.GetHandle());
			}

			return vertices;
		}
	}
}