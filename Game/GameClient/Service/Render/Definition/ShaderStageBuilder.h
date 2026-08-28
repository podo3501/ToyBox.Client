#pragma once
#include "GameClient/Service/Render/Definition/Shader/ShaderTypes.h"

struct ShaderAsset;
struct ShaderDesc;

ShaderDesc BuildShader(ShaderType type, std::shared_ptr<ShaderAsset> asset);

