#pragma once
#include "GameClient/Service/Render/Definition/Shader/ShaderTypes.h"
#include <vector>
#include <memory>
#include <string>
#include <utility>

struct ShaderAsset;

enum class ShaderStage
{
    Vertex,
    Pixel,
    Compute
};

struct ShaderStageDesc
{
    ShaderStage stage;
    std::string entry;
    std::string target;
};

struct ShaderDesc
{
    std::shared_ptr<ShaderAsset> asset;
    std::vector<ShaderStageDesc> stages;
};

using RegistryShaderDesc = std::pair<ShaderID, ShaderDesc>;