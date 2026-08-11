#include "pch.h"
#include "BuiltinBrush.h"
#include "../Repository/BrushRepository.h"

BrushHandle CreateBuiltinBrush(BrushRepository* repository)
{
    BrushDesc desc{ Core::ResourceID::MakeBuiltin("Brush") };
    return repository->AcquireFromAsset(desc, nullptr);
}