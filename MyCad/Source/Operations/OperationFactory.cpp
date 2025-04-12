#include "OperationFactory.h"
#include <exception>

void OperationFactory::HandleInput()
{
    static const std::vector<std::pair<OperationType, ImGuiKey>> operationBindings = {
        { OperationType::Translation, ImGuiKey_T },
        { OperationType::Rotation, ImGuiKey_R },
        { OperationType::Scaling, ImGuiKey_S },
        { OperationType::Camera, ImGuiKey_C },
    };

    for (const auto& operationBinding : operationBindings)
    {
        if (ImGui::IsKeyPressed(operationBinding.second))
        {
            operationType = operationBinding.first;
            lastKey = ImGuiKey_1;
            OperationUpdated = true;
            return;
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        ImGuiKey key = static_cast<ImGuiKey>(ImGuiKey_1 + i);
        if (ImGui::IsKeyPressed(key))
        {
            lastKey = key;
            OperationUpdated = true;
        }
    }
}

std::shared_ptr<Operation> OperationFactory::CreateOperation(OperationParameters& params)
{
    if (!OperationUpdated)
    {
        return lastOperation;
    }

    switch (operationType)
    {
    case OperationType::Camera:
    {
        lastOperation = nullptr;
        break;
    }
    case OperationType::Translation:
    {
        lastOperation = std::make_shared<TranslationAlongAxesOperation>(params.selected);
        break;
    }
    case OperationType::Rotation:
    {
        switch (lastKey)
        {
        case ImGuiKey_1:
        {
            lastOperation = std::make_shared<RotationAroundAxesOperation>(params.selected);
            break;
        }
        case ImGuiKey_2:
        {
            lastOperation = std::make_shared<RotationAroundPointOperation>(params.selected, params.cursor);
            break;
        }
        }
        break;
    }
    case OperationType::Scaling:
    {
        switch (lastKey)
        {
        case ImGuiKey_1:
        {
            lastOperation = std::make_shared<ScalingUniformly>(params.selected);
            break;
        }
        case ImGuiKey_2:
        {
            lastOperation = std::make_shared<ScalingAroundPoint>(params.selected, params.cursor);
            break;
        }
        }
        break;
    }
    }

    OperationUpdated = false;
    return lastOperation;
}
