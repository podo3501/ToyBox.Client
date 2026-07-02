#pragma once
#include "../Desc/ShaderStageBuilder.h"

struct IAssetAsyncLoader;
std::vector<BuiltinShaderDesc> CreateBuiltinShaders(IAssetAsyncLoader* asyncLoader);