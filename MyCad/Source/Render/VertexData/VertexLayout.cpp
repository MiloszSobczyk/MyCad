#include "VertexLayout.h"


void VertexLayoutRegistry::Register(std::type_index type, std::shared_ptr<VertexLayout> layout)
{
    layouts()[type] = layout;
}

const VertexLayout* VertexLayoutRegistry::Get(std::type_index type) 
{
    auto it = layouts().find(type);
    return it != layouts().end() ? it->second.get() : nullptr;
}

std::unordered_map<std::type_index, std::shared_ptr<VertexLayout>>& VertexLayoutRegistry::layouts() 
{
    static std::unordered_map<std::type_index, std::shared_ptr<VertexLayout>> map;
    return map;
}
