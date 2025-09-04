#pragma once

#include "Core/Scene/Scene.h"
#include "Core/Scene/Entity.h"
#include "ECS/Components/Components.h"

#include <iostream>
#include <numbers>

namespace ShapeCreator
{
	Entity CreatePoint(Ref<Scene> scene);

	inline void AddAsTargets(entt::entity targetHandle, std::vector<entt::entity> notifiers, Ref<Scene> scene) 
	{
		for (auto pointHandle : notifiers)
		{
			Entity pointEntity{ pointHandle, scene.get() };
			auto& nc = pointEntity.EmplaceComponent<NotificationComponent>();
			nc.AddToNotify(targetHandle);
		}
	}
}

namespace Curve
{
	namespace C0
	{
		inline void PadBernsteinVertices(std::vector<Algebra::Vector4>& vertices)
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

		inline std::vector<Algebra::Vector4> Update(CurveComponent& curve, Ref<Scene> scene)
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

			PadBernsteinVertices(result);

			return result;
		}
	}
	namespace C2
	{
		inline std::vector<Algebra::Vector4> Update(CurveComponent& curveComponent, Ref<Scene> scene)
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

		inline std::vector<Algebra::Vector4> Update(CurveComponent& curveComponent, Ref<Scene> scene)
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

namespace Surface
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

	namespace Gregory
	{
		struct SurfaceEdge
		{
			std::array<std::array<entt::entity, 4>, 4> controlPointHandles{};
		};

		struct Triangle
		{
			std::array<SurfaceEdge, 3> surfaceEdges;
		};

		inline void printSurfaceEdge(const SurfaceEdge& edge, Ref<Scene> scene)
		{
			for (std::size_t i = 0; i < edge.controlPointHandles.size(); i++)
			{
				for (std::size_t j = 0; j < edge.controlPointHandles[i].size(); j++)
				{
					Entity point{ edge.controlPointHandles[i][j], scene.get() };
					std::cout << point.GetComponent<IdComponent>().id << " ";
				}
				std::cout << "\n";
			}
		}

		inline void PrintTriangle(const Triangle& triangle, Ref<Scene> scene)
		{
			for (std::size_t i = 0; i < triangle.surfaceEdges.size(); i++)
			{
				std::cout << "SurfaceEdge " << i << ":\n";
				printSurfaceEdge(triangle.surfaceEdges[i], scene);
				std::cout << "----------------------\n";
			}
			std::cout << "IF BUGS HAPPEN. YOU MAY NEED TO FLIP THE EDGES!!!";
		}


		inline Triangle FindCycles(Ref<Scene> scene)
		{
			auto surfaces = scene->GetAllEntitiesWith<BezierSurfaceComponent>();

			for (auto surface : surfaces)
			{
				std::cout << surface.GetComponent<IdComponent>().id << '\n';
			}

			auto surface2 = surfaces[0].GetComponent<BezierSurfaceComponent>();
			SurfaceEdge edge2;

			for (int i = 0; i <= 15; ++i)
			{
				edge2.controlPointHandles[i / 4][i % 4] = surface2.pointHandles[i];
			}

			auto surface1 = surfaces[1].GetComponent<BezierSurfaceComponent>();
			SurfaceEdge edge1;

			for (int i = 0; i <= 15; ++i)
			{
				int j = 15 - i;
				edge1.controlPointHandles[i / 4][i % 4] = surface1.pointHandles[j];
			}

			auto surface0 = surfaces[2].GetComponent<BezierSurfaceComponent>();
			SurfaceEdge edge0;

			for (int i = 0; i <= 15; ++i)
			{
				int j = 15 - i;
				edge0.controlPointHandles[i / 4][i % 4] = surface0.pointHandles[j];
			}

			Triangle triangle{ { edge2, edge1, edge0 } };

			for (auto sEdge : triangle.surfaceEdges)
			{
				for (auto handleRow : sEdge.controlPointHandles)
				{
					for (auto handle : handleRow)
					{
						Entity point{ handle, scene.get() };
						std::cout << point.GetComponent<IdComponent>().id << ' ';
					}
					std::cout << '\n';
				}
				std::cout << "----\n";
			}

			return triangle;
		}

		// 16 + 31
		// 34 + 40
		// 37 + 13

	}
}