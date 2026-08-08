#include "pch.h"
#include "BuiltinBrush.h"
#include "../Repository/Brush/BrushRepository.h"

BrushHandle CreateBuiltinBrush(BrushRepository* repository)
{
    BrushDesc desc;
    return repository->GetOrCreate(desc);
}