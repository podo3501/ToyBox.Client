#pragma once
#include "GameClient/Service/Render/Definition/View/RenderState.h"

struct ShaderAsset;
ShaderDesc BuildShader(ShaderType type, std::shared_ptr<ShaderAsset> asset);

