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

namespace Surfaces
{
	inline std::vector<Algebra::Vector4> SetupControlPoints(
		BezierSurfaceComponent& bsc, Algebra::Vector4 position, float width, float height)
	{
		int rows = bsc.GetRows();
		int columns = bsc.GetColumns();
		bool c2 = bsc.C2;
		std::vector<Algebra::Vector4> controlPoints;
		controlPoints.reserve(rows * columns);

		switch (bsc.connectionType)
		{
		case ConnectionType::Flat:
		{
			float dx = width / static_cast<float>(columns - 1);
			float dy = height / static_cast<float>(rows - 1);
			Algebra::Vector4 startPos = position - Algebra::Vector4(width / 2.f, height / 2.f, 0.f);

			for (int i = 0; i < rows; ++i)
			{
				for (int j = 0; j < columns; ++j)
				{
					Algebra::Vector4 point = startPos + Algebra::Vector4(j * dx, i * dy, 0.f);
					point.w = 1.f;
					controlPoints.push_back(point);
				}
			}
			break;
		}

		case ConnectionType::Columns:
		{
			float dHeight = height / static_cast<float>(rows - 1);
			float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(columns - 1);
			Algebra::Vector4 startPos = position - Algebra::Vector4(0.f, 0.f, height / 2.f);

			for (int i = 0; i < rows; ++i)
			{
				for (int j = 0; j < columns - (c2 ? 3 : 1); ++j)
				{
					Algebra::Vector4 point = startPos + Algebra::Vector4(0.f, 0.f, i * dHeight) +
						Algebra::Matrix4::RotationZ(dAngle * j) *
						Algebra::Vector4(width / 2.f, 0.f, 0.f);
					point.w = 1.f;
					controlPoints.push_back(point);
				}
				controlPoints.push_back(controlPoints[i * columns + 0]);
				if (c2)
				{
					controlPoints.push_back(controlPoints[i * columns + 1]);
					controlPoints.push_back(controlPoints[i * columns + 2]);
				}
			}
			break;
		}

		case ConnectionType::Rows:
		{
			float dHeight = height / static_cast<float>(columns - 1);
			float dAngle = 2.f * std::numbers::pi_v<float> / static_cast<float>(rows - 1);
			Algebra::Vector4 startPos = position - Algebra::Vector4(height / 2.f, 0.f, 0.f);

			for (int i = 0; i < rows - 3; ++i)
			{
				for (int j = 0; j < columns; ++j)
				{
					Algebra::Vector4 point = startPos +
						Algebra::Vector4(j * dHeight, 0.f, 0.f) +
						Algebra::Matrix4::RotationX(dAngle * i) *
						Algebra::Vector4(0.f, width / 2.f, 0.f);
					point.w = 1.f;
					controlPoints.push_back(point);
				}
			}
			for (int i = 0; i < (c2 ? 3 : 1); ++i)
			{
				for (int j = 0; j < columns; ++j)
				{
					controlPoints.push_back(controlPoints[i * columns + j]);
				}
			}
			break;
		}
		}

		return controlPoints;
	}

	inline void InitializePatchPolylinePoints(Entity polylineEntity, std::vector<entt::entity> pointHandles,
		Ref<Scene> scene, bool addToNotify = true)
	{
		std::vector<entt::entity> polylinePointHandles = {
			// Rows
			pointHandles[0],
			pointHandles[1],
			pointHandles[2],
			pointHandles[3],

			pointHandles[7],
			pointHandles[6],
			pointHandles[5],
			pointHandles[4],

			pointHandles[8],
			pointHandles[9],
			pointHandles[10],
			pointHandles[11],

			pointHandles[15],
			pointHandles[14],
			pointHandles[13],
			pointHandles[12],

			// Columns
			pointHandles[12],
			pointHandles[8],
			pointHandles[4],
			pointHandles[0],

			pointHandles[1],
			pointHandles[5],
			pointHandles[9],
			pointHandles[13],

			pointHandles[14],
			pointHandles[10],
			pointHandles[6],
			pointHandles[2],

			pointHandles[3],
			pointHandles[7],
			pointHandles[11],
			pointHandles[15]
		};

		polylineEntity.EmplaceTag<IsInvisibleTag>();
		// TODO: I'm retarded
		polylineEntity.EmplaceComponent<VirtualComponent>();
		auto& polylineComponent = polylineEntity.GetComponent<PolylineComponent>();
		polylineComponent.pointHandles = polylinePointHandles;

		if (addToNotify)
		{
			for (auto handle : polylinePointHandles)
			{
				Entity pointEntity{ handle, scene.get() };
				auto& notificationComponent = pointEntity.GetComponent<NotificationComponent>();
				notificationComponent.AddToNotify(polylineEntity.GetHandle());
			}
		}
	}

	namespace C0
	{
		inline std::vector<Algebra::Vector4> UpdatePatch(PatchComponent& patch, Ref<Scene> scene)
		{
			std::vector<Algebra::Vector4> result;
			result.reserve(patch.pointHandles.size());

			for (auto handle : patch.pointHandles)
			{
				Entity pointEntity{ handle, scene.get() };
				result.push_back(pointEntity.GetComponent<TranslationComponent>().translation);
			}

			return result;
		}
	}

	namespace C2
	{
		inline std::vector<Algebra::Vector4> UpdatePatch(PatchComponent& patch, Ref<Scene> scene,
			PolylineComponent& bernsteinPolyline)
		{
			static constexpr float A[4][4] = {
				{ 1.f / 6.f, 4.f / 6.f, 1.f / 6.f, 0.f },
				{ 0.f,       4.f / 6.f, 2.f / 6.f, 0.f },
				{ 0.f,       2.f / 6.f, 4.f / 6.f, 0.f },
				{ 0.f,       1.f / 6.f, 4.f / 6.f, 1.f / 6.f }
			};

			std::array<std::array<Algebra::Vector4, 4>, 4> P{};
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					P[i][j] = Entity{ patch.pointHandles[i * 4 + j], scene.get() }
					.GetComponent<TranslationComponent>().translation;
				}
			}

			std::array<std::array<Algebra::Vector4, 4>, 4> Q{};
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					for (int k = 0; k < 4; ++k)
					{
						Q[i][j] += A[i][k] * P[k][j];
					}
				}
			}

			std::vector<Algebra::Vector4> bernsteinPoints;
			bernsteinPoints.reserve(16);

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					Algebra::Vector4 b;
					for (int k = 0; k < 4; ++k)
					{
						b += Q[i][k] * A[j][k];
					}
					bernsteinPoints.push_back(b);
				}
			}

			for (int i = 0; i < 16; ++i)
			{
				Entity point{ bernsteinPolyline.pointHandles[i], scene.get() };
				point.GetComponent<TranslationComponent>().SetTranslation(bernsteinPoints[i]);
				point.RemoveComponent<IsNotifiedTag>();
			}

			return bernsteinPoints;
		}
	}

	namespace Patches
	{
		struct Edge
		{
			std::vector<entt::entity> points;

			entt::entity Start() const
			{
				return points.front();
			}

			entt::entity End() const
			{
				return points.back();
			}

			void Reverse()
			{
				std::reverse(points.begin(), points.end());
			}
		};

		struct SurfaceEdge
		{
			entt::entity surface;
			int edgeIndex; // 0=bottom,1=right,2=top,3=left
			Edge edge;
		};

		inline std::vector<std::array<SurfaceEdge, 3>> FindCycles(Ref<Scene> scene)
		{
			std::vector<SurfaceEdge> allEdges;

			auto surfaces = scene->GetAllEntitiesWith<BezierSurfaceComponent, IsSelectedTag>();
			for (auto surface : surfaces)
			{
				auto& bsc = surface.GetComponent<BezierSurfaceComponent>();
				int rows = bsc.GetRows();
				int columns = bsc.GetColumns();

				// Bottom edge (left -> right)
				Edge bottom;
				for (int col = 0; col < columns; ++col)
				{
					bottom.points.push_back(bsc.pointHandles[col]);
				}

				// Right edge (bottom -> top)
				Edge right;
				for (int row = 0; row < rows; ++row)
				{
					right.points.push_back(bsc.pointHandles[row * columns + (columns - 1)]);
				}

				// Top edge (right -> left)
				Edge top;
				for (int col = columns - 1; col >= 0; --col)
				{
					top.points.push_back(bsc.pointHandles[(rows - 1) * columns + col]);
				}

				// Left edge (top -> bottom)
				Edge left;
				for (int row = rows - 1; row >= 0; --row)
				{
					left.points.push_back(bsc.pointHandles[row * columns]);
				}

				allEdges.push_back({ surface.GetHandle(), 0, bottom });
				allEdges.push_back({ surface.GetHandle(), 1, right });
				allEdges.push_back({ surface.GetHandle(), 2, top });
				allEdges.push_back({ surface.GetHandle(), 3, left });
			}

			if (allEdges.empty())
			{
				return {};
			}

			std::vector<std::unordered_set<int>> adjacency;
			adjacency.resize(allEdges.size());

			for (size_t i = 0; i < allEdges.size(); ++i)
			{
				for (size_t j = i + 1; j < allEdges.size(); ++j)
				{
					auto aStart = allEdges[i].edge.Start();
					auto aEnd = allEdges[i].edge.End();
					auto bStart = allEdges[j].edge.Start();
					auto bEnd = allEdges[j].edge.End();

					if (aStart == bStart || aStart == bEnd || aEnd == bStart || aEnd == bEnd)
					{
						adjacency[i].insert(static_cast<int>(j));
						adjacency[j].insert(static_cast<int>(i));
					}
				}
			}

			auto CanFormClosedChain =
				[&](const SurfaceEdge& A, const SurfaceEdge& B, const SurfaceEdge& C) -> bool
				{
					entt::entity a1 = A.edge.Start();
					entt::entity a2 = A.edge.End();
					entt::entity b1 = B.edge.Start();
					entt::entity b2 = B.edge.End();
					entt::entity c1 = C.edge.Start();
					entt::entity c2 = C.edge.End();

					std::array<std::pair<entt::entity, entt::entity>, 2> Ao
						= { std::make_pair(a1, a2), std::make_pair(a2, a1) };
					std::array<std::pair<entt::entity, entt::entity>, 2> Bo
						= { std::make_pair(b1, b2), std::make_pair(b2, b1) };
					std::array<std::pair<entt::entity, entt::entity>, 2> Co
						= { std::make_pair(c1, c2), std::make_pair(c2, c1) };

					for (auto& aOri : Ao)
					{
						for (auto& bOri : Bo)
						{
							for (auto& cOri : Co)
							{
								if (aOri.second == bOri.first &&
									bOri.second == cOri.first &&
									cOri.second == aOri.first)
								{
									return true;
								}
							}
						}
					}

					return false;
				};

			std::set<std::array<uint64_t, 3>> canonicalTriangles;
			std::vector<std::array<SurfaceEdge, 3>> resultTriangles;

			for (size_t i = 0; i < adjacency.size(); ++i)
			{
				for (int j : adjacency[i])
				{
					if (static_cast<size_t>(j) <= i) continue;
					for (int k : adjacency[j])
					{
						if (static_cast<size_t>(k) <= static_cast<size_t>(j)) continue;
						if (adjacency[k].count(static_cast<int>(i)) == 0) continue;

						if (CanFormClosedChain(allEdges[i], allEdges[j], allEdges[k]))
						{
							auto encode = [&](const SurfaceEdge& se) -> uint64_t
								{
									return (static_cast<uint64_t>(se.surface) << 32) | se.edgeIndex;
								};
							std::array<uint64_t, 3> key = { encode(allEdges[i]), encode(allEdges[j]), encode(allEdges[k]) };
							std::sort(key.begin(), key.end());
							if (canonicalTriangles.insert(key).second)
							{
								resultTriangles.push_back({ allEdges[i], allEdges[j], allEdges[k] });
							}
						}
					}
				}
			}

			return resultTriangles;

		}

		// 16 + 31
		// 34 + 40
		// 37 + 13

	}
}