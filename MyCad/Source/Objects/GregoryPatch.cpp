#include "GregoryPatch.h"
#include "Core/App.h"
#include "Managers/ShaderManager.h"
#include "Point.h"

GregoryPatch::GregoryPatch(std::array<std::shared_ptr<Point>, 4> edge1, std::array<std::shared_ptr<Point>, 4> edge2,
	std::array<std::shared_ptr<Point>, 4> edge3)
	: renderer(VertexDataType::PositionVertexData), edge1(std::move(edge1)), edge2(std::move(edge2)), edge3(std::move(edge3))
{
	name = "GregoryPatch_" + std::to_string(id);
	Update();
}

void GregoryPatch::Update()
{
    Algebra::Vector4 v1 = edge1[0]->GetTranslationComponent()->GetTranslation();
    Algebra::Vector4 c12 = edge1[1]->GetTranslationComponent()->GetTranslation();
    Algebra::Vector4 c13 = edge1[2]->GetTranslationComponent()->GetTranslation();
    Algebra::Vector4 v2 = edge1[3]->GetTranslationComponent()->GetTranslation();

    Algebra::Vector4 c23 = edge2[1]->GetTranslationComponent()->GetTranslation();
    Algebra::Vector4 c24 = edge2[2]->GetTranslationComponent()->GetTranslation();
    Algebra::Vector4 v3 = edge2[3]->GetTranslationComponent()->GetTranslation();

    Algebra::Vector4 c31 = edge3[1]->GetTranslationComponent()->GetTranslation();
    Algebra::Vector4 c32 = edge3[2]->GetTranslationComponent()->GetTranslation();

    Algebra::Vector4 t_v1_u = 3.0f * (edge1[1]->GetTranslationComponent()->GetTranslation() - edge1[0]->GetTranslationComponent()->GetTranslation());
    Algebra::Vector4 t_v1_w = 3.0f * (edge3[3]->GetTranslationComponent()->GetTranslation() - edge3[2]->GetTranslationComponent()->GetTranslation());

    Algebra::Vector4 t_v2_u = 3.0f * (edge2[1]->GetTranslationComponent()->GetTranslation() - edge2[0]->GetTranslationComponent()->GetTranslation());
    Algebra::Vector4 t_v2_w = 3.0f * (edge1[3]->GetTranslationComponent()->GetTranslation() - edge1[2]->GetTranslationComponent()->GetTranslation());

    Algebra::Vector4 t_v3_u = 3.0f * (edge3[1]->GetTranslationComponent()->GetTranslation() - edge3[0]->GetTranslationComponent()->GetTranslation());
    Algebra::Vector4 t_v3_w = 3.0f * (edge2[3]->GetTranslationComponent()->GetTranslation() - edge2[2]->GetTranslationComponent()->GetTranslation());

    Algebra::Vector4 twist1 = 0.5f * (t_v1_u + t_v1_w);
    Algebra::Vector4 twist2 = 0.5f * (t_v2_u + t_v2_w);
    Algebra::Vector4 twist3 = 0.5f * (t_v3_u + t_v3_w);

    std::vector<PositionVertexData> cp;

    cp.push_back({ .Position = v1 });
    cp.push_back({ .Position = c12 });
    cp.push_back({ .Position = c13 });
    cp.push_back({ .Position = v2 });
    cp.push_back({ .Position = c23 });
    cp.push_back({ .Position = c24 });
    cp.push_back({ .Position = v3 });
    cp.push_back({ .Position = c31 });
    cp.push_back({ .Position = c32 });
    cp.push_back({ .Position = (v1 + v2 + v3 + twist1 + twist2 + twist3) / 6.0f });

    renderer.SetVertices(cp);
}

void GregoryPatch::Render()
{
    renderer.SetPatchParameters(10);

    auto shader = ShaderManager::GetInstance().GetShader(ShaderName::GregoryPatch);

    shader->Bind();
    shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
    shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
    shader->SetUniformInt("u_tessLevel", tessLevelU);

    renderer.Render(GL_LINES);

    shader->Unbind();

    renderer.SetPatchParameters(4);
}

void GregoryPatch::ReplacePoint(std::shared_ptr<Point> oldPoint, std::shared_ptr<Point> newPoint)
{
}

void GregoryPatch::RenderUI()
{
}

json GregoryPatch::Serialize() const
{
    return json();
}
