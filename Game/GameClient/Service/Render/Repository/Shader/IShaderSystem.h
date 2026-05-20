#pragma once
#include "GameClient/Service/Asset/Assets/ShaderAsset.h"

enum class ShaderID;

struct IShaderSystem
{
    virtual ~IShaderSystem() = default;
    virtual bool Register(ShaderID shaderID, std::shared_ptr<ShaderAsset> asset) = 0;
};
