#pragma once
#include "../Definition/BrushDesc.h"
#include "../Handle/BrushHandle.h"

class BrushRepository;

BrushHandle CreateBuiltinBrush(BrushRepository* repository);
