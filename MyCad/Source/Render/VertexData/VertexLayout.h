#pragma once

#include <unordered_map>
#include <typeindex>
#include <memory>

class VertexLayout
{
public:
    virtual ~VertexLayout() = default;
    virtual void SetupVAO() const = 0;
};

class VertexLayoutRegistry 
{
public:
    static void Register(std::type_index type, std::shared_ptr<VertexLayout> layout);
    static const VertexLayout* Get(std::type_index type);

private:
    static std::unordered_map<std::type_index, std::shared_ptr<VertexLayout>>& layouts();
};

