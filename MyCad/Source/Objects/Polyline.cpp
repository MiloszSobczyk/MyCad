#include "Polyline.h"
#include "Core/App.h"
#include "Managers/ShaderManager.h"
#include "Managers/IdManager.h"

#include <GLFW/glfw3.h>

Polyline::Polyline()
    : renderer(VertexDataType::PositionVertexData)
{
    name = "Polyline_" + std::to_string(id);
}

Polyline::Polyline(std::vector<std::weak_ptr<Point>> points)
    : renderer(VertexDataType::PositionVertexData), points(points)
{
    name = "Polyline_" + std::to_string(id);
    UpdatePoints();
}

void Polyline::Render()
{
    if (points.size() > 0)
    {
        auto shader = ShaderManager::GetInstance().GetShader(ShaderName::Default);
        
        shader->Bind();
        shader->SetUniformVec4f("u_color", App::useStereoscopy ? ColorPalette::Get(Color::White) : color);
        shader->SetUniformMat4f("u_viewMatrix", App::camera.GetViewMatrix());
        shader->SetUniformMat4f("u_projectionMatrix", App::projectionMatrix);
        shader->SetUniformMat4f("u_modelMatrix", GetModelMatrix());

        renderer.Render(GL_LINES);

        shader->Unbind();
    }
}

void Polyline::RenderUI()
{
    Shape::RenderUI();

    if (points.empty())
    {
        ImGui::Text("No points.");
        return;
    }

    bool changed = false;
    ImGui::SeparatorText("Control Points");

    for (size_t i = 0; i < points.size(); ++i)
    {
        if (auto point = points[i].lock())
        {
            ImGui::PushID(static_cast<int>(i));

            if (ImGui::Button("X"))
            {
                RemovePoint(point);
                changed = true;
                ImGui::PopID();
                break;
            }

            ImGui::SameLine();
            ImGui::Text("%s", point->GetName().c_str());

            if (i < points.size() - 1)
            {
                ImGui::SameLine();
                if (ImGui::ArrowButton("down", ImGuiDir_Down))
                {
                    SwapPoints(i, i + 1);
                    changed = true;
                    ImGui::PopID();
                    break;
                }
            }

            if (i > 0)
            {
                ImGui::SameLine();
                if (ImGui::ArrowButton("up", ImGuiDir_Up))
                {
                    SwapPoints(i, i - 1);
                    changed = true;
                    ImGui::PopID();
                    break;
                }
            }

            ImGui::PopID();
        }
    }

    if (changed)
    {
        UpdatePoints();
    }
}

void Polyline::UpdatePoints()
{
    if (points.empty())
    {
        return;
    }
    
    std::vector<PositionVertexData> vertices;
    std::vector<unsigned int> indices;
    for (auto it = points.begin(); it != points.end();)
    {
        if (auto point = it->lock())
        {
            auto position = point->GetTranslationComponent()->GetTranslation();
            position.w = 1.f;
            vertices.push_back(PositionVertexData{ .Position = position });
            ++it;
        }
        else
        {
            it = points.erase(it);
        }
    }
    
    for (unsigned int i = 0; i < vertices.size() - 1; ++i)
    {
        indices.push_back(i);
        indices.push_back(i + 1);
    }
 
    renderer.SetVertices(vertices);
    renderer.SetIndices(indices);
}

void Polyline::AddPoint(const std::shared_ptr<Point>& point)
{
    if (!point) return;

    for (const auto& p : points)
    {
        if (auto sp = p.lock())
        {
            if (sp == point)
                return;
        }
    }

    points.push_back(point);
    point->AddObserver(shared_from_this());

    UpdatePoints();
}

void Polyline::AddPoint(const std::weak_ptr<Point>& point)
{
    auto spPoint = point.lock();
    if (!spPoint)
        return;

    for (const auto& w : points)
    {
        if (auto spExisting = w.lock())
        {
            if (spExisting == spPoint)
                return;
        }
    }

    points.push_back(point);
    spPoint->AddObserver(shared_from_this());

    UpdatePoints();
}


void Polyline::RemovePoint(const std::shared_ptr<Point>& point)
{
    points.erase(
        std::remove_if(points.begin(), points.end(),
            [&](const std::weak_ptr<Point>& p) {
                auto sp = p.lock();
                return !sp || sp == point;
            }),
        points.end()
    );

    if (point)
    {
        point->RemoveObserver(shared_from_this());
    }

    UpdatePoints();
}

void Polyline::SwapPoints(int index1, int index2)
{
	if (index1 < 0 || index2 < 0 || index1 >= points.size() || index2 >= points.size())
		return;
    std::swap(points[index1], points[index2]);
    UpdatePoints();
}

void Polyline::ClearPoints()
{
    points.clear();
}

void Polyline::OnNotified()
{
    UpdatePoints();
}

void Polyline::InitFromPoints()
{
    for (const auto& p : points)
    {
        if (auto sp = p.lock())
        {
            sp->AddObserver(shared_from_this());
        }
    }
}

json Polyline::Serialize() const
{
    json j;

    j["objectType"] = "chain";
    j["id"] = static_cast<unsigned int>(id);
    if (!name.empty())
    {
        j["name"] = name;
    }
    
    json cp = json::array();
	for (const auto& p : points)
	{
		if (auto point = p.lock())
		{
            cp.push_back(json{ {"id", static_cast<unsigned int>(point->GetId())} });
		}
	}
	j["controlPoints"] = cp;

    return j;
}

std::shared_ptr<Polyline> Polyline::Deserialize(const json& j)
{
    auto pl = std::make_shared<Polyline>();

    pl->id = j.at("id").get<unsigned int>();
    if (j.contains("name"))
    {
        pl->name = j.at("name").get<std::string>();
    }
    else
    {
		pl->name = "Polyline_" + std::to_string(pl->id);
    }

    for (const auto& cp : j.at("controlPoints"))
    {
        unsigned int pid = cp.at("id").get<unsigned int>();
        auto shape = IdManager::GetById(pid);
        if (auto pt = std::dynamic_pointer_cast<Point>(shape))
        {
            pl->AddPoint(pt);
        }
    }

    return pl;
}
