#include "OperationFactory.h"
#include <exception>

std::unique_ptr<Operation> OperationFactory::CreateOperation(OperationType type, OperationParameters& params)
{
	switch (type)
	{
	case OperationType::Translation:
		return std::make_unique<TranslationAlongAxesOperation>(params.selected);
	default:
		throw std::invalid_argument("Unknown OperationType");
	}
}
