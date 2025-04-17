#pragma once

#include <vector>
#include <memory>
#include <algorithm>
#include <string>

#include "imgui/imgui.h"

template <typename T>
concept HasGetName = requires(T t) 
{
    { t.GetName() } -> std::convertible_to<std::string>;
};

template <typename T>
class Collection
{
private:
    std::vector<std::shared_ptr<T>> items;

protected:
    bool itemsChanged = false;

public:
    Collection() : items()
    {
    }

    inline virtual void Clear()
    {
        items.clear();
    }

    inline virtual void AddItem(std::shared_ptr<T>& item)
    {
        if (!IsPresent(item))
        {
            items.push_back(item);
        }
    }

    inline virtual void RemoveItem(std::shared_ptr<T>& item)
    {
        items.erase(std::remove(items.begin(), items.end(), item), items.end());
    }

    inline virtual void ToggleItem(std::shared_ptr<T>& item)
    {
        if (IsPresent(item))
        {
            RemoveItem(item);
        }
        else
        {
            AddItem(item);
        }
    }

    inline virtual void Swap(int index1, int index2)
    {
        if (index1 < 0 || index1 >= static_cast<int>(items.size()) ||
            index2 < 0 || index2 >= static_cast<int>(items.size()) ||
            index1 == index2) 
        {
            return;
        }

        std::swap(items[index1], items[index2]);
    }

    inline bool IsPresent(std::shared_ptr<T>& item) const
    {
        return std::find(items.begin(), items.end(), item) != items.end();
    }

    inline std::shared_ptr<T> GetAt(int index) const
    {
        if (index >= 0 && index < static_cast<int>(items.size()))
        {
            return items[index];
        }
        return nullptr;
    }

    inline bool IsEmpty() const { return items.empty(); }
    inline int Size() const { return static_cast<int>(items.size()); }

    inline auto begin() { return items.begin(); }
    inline auto end() { return items.end(); }

    inline auto begin() const { return items.begin(); }
    inline auto end() const { return items.end(); }

    inline void RenderUI() 
    {
        if (items.empty())
        {
            ImGui::Text("No items.");
            return;
        }

        ImGui::SeparatorText("Items");

        for (size_t i = 0; i < items.size(); ++i)
        {
            if (auto item = items[i].lock())
            {
                ImGui::PushID(static_cast<int>(i));

                if (ImGui::Button("X"))
                {
                    RemoveItem(item);
                    itemsChanged = true;
                    ImGui::PopID();
                    break;
                }

                ImGui::SameLine();
                ImGui::Text("%s", item->GetName().c_str());

                if (i > 0)
                {
                    ImGui::SameLine();
                    if (ImGui::ArrowButton("up", ImGuiDir_Up))
                    {
                        Swap(i, i - 1);
                        itemsChanged = true;
                        ImGui::PopID();
                        break;
                    }
                }

                if (i < items.size() - 1)
                {
                    ImGui::SameLine();
                    if (ImGui::ArrowButton("down", ImGuiDir_Down))
                    {
                        Swap(i, i + 1);
                        itemsChanged = true;
                        ImGui::PopID();
                        break;
                    }
                }

                ImGui::PopID();
            }
        }
    }
};
