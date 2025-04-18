#pragma once

#include <vector>
#include <algorithm>
#include <string>

#include "imgui/imgui.h"



template <typename T>
class Collection
{
protected:
    std::vector<T> items;
    bool itemsChanged = false;

public:
    Collection() = default;

    inline virtual void Clear()
    {
        items.clear();
    }

    inline virtual void AddItem(const T& item)
    {
        if (!IsPresent(item))
        {
            items.push_back(item);
        }
    }

    inline virtual void RemoveItem(const T& item)
    {
        items.erase(std::remove(items.begin(), items.end(), item), items.end());
    }

    inline virtual void ToggleItem(const T& item)
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

    inline bool IsPresent(const T& item) const
    {
        return std::find(items.begin(), items.end(), item) != items.end();
    }

    inline const T* GetAt(int index) const
    {
        if (index >= 0 && index < static_cast<int>(items.size()))
        {
            return &items[index];
        }
        return nullptr;
    }

    inline bool IsEmpty() const { return items.empty(); }
    inline int Size() const { return static_cast<int>(items.size()); }

    inline auto begin() { return items.begin(); }
    inline auto end() { return items.end(); }

    inline auto begin() const { return items.begin(); }
    inline auto end() const { return items.end(); }

    //inline void RenderUI()
    //{
    //    if (items.empty())
    //    {
    //        ImGui::Text("No items.");
    //        return;
    //    }

    //    ImGui::SeparatorText("Items");

    //    for (size_t i = 0; i < items.size(); ++i)
    //    {
    //        ImGui::PushID(static_cast<int>(i));

    //        if (ImGui::Button("X"))
    //        {
    //            RemoveItem(items[i]);
    //            itemsChanged = true;
    //            ImGui::PopID();
    //            break;
    //        }

    //        ImGui::SameLine();
    //        ImGui::Text("%s", items[i].GetName().c_str());

    //        if (i > 0)
    //        {
    //            ImGui::SameLine();
    //            if (ImGui::ArrowButton("up", ImGuiDir_Up))
    //            {
    //                Swap(i, i - 1);
    //                itemsChanged = true;
    //                ImGui::PopID();
    //                break;
    //            }
    //        }

    //        if (i < items.size() - 1)
    //        {
    //            ImGui::SameLine();
    //            if (ImGui::ArrowButton("down", ImGuiDir_Down))
    //            {
    //                Swap(i, i + 1);
    //                itemsChanged = true;
    //                ImGui::PopID();
    //                break;
    //            }
    //        }

    //        ImGui::PopID();
    //    }
    //}
};
