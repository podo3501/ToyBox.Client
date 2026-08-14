#pragma once
#include "../Handle/BrushHandle.h"

template <typename TagT> 
class ResourceRepository;

BrushHandle CreateBuiltinBrush(ResourceRepository<BrushTag>& repository);
