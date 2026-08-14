#include "pch.h"
#include "BuiltinBrush.h"
#include "../Repository/Base/ResourceRepository.h"
#include "../Definition/BrushDesc.h"

BrushHandle CreateBuiltinBrush(ResourceRepository<BrushTag>& repository)
{
    BrushDesc desc{ Core::ResourceID::MakeBuiltin("Brush") };
    return repository.AcquireFromAsset(desc, nullptr);
}