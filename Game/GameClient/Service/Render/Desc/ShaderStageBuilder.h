#pragma once
#include "GameClient/Service/Render/Desc/RenderState.h"

struct ShaderAsset;
ShaderDesc BuildShader(ShaderType type, std::shared_ptr<ShaderAsset> asset);

