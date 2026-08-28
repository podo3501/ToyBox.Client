#include "pch.h"
#include "ShaderStageBuilder.h"
#include "ShaderDesc.h"

static ShaderStageDesc VS(std::string entry = "VSMain")
{
    return
    {
        ShaderStage::Vertex,
        std::move(entry),
        "vs_6_6"
    };
}

static ShaderStageDesc PS(std::string entry = "PSMain")
{
    return
    {
        ShaderStage::Pixel,
        std::move(entry),
        "ps_6_6"
    };
}

static ShaderStageDesc CS(std::string entry = "CSMain")
{
    return
    {
        ShaderStage::Compute,
        std::move(entry),
        "cs_6_6"
    };
}

static ShaderDesc Build(
    std::shared_ptr<ShaderAsset> asset,
    std::initializer_list<ShaderStageDesc> stages)
{
    ShaderDesc desc;
    desc.asset = std::move(asset);
    desc.stages.assign(stages.begin(), stages.end());
    return desc;
}

ShaderDesc BuildShader(ShaderType type, std::shared_ptr<ShaderAsset> asset)
{
    switch (type)
    {
    case ShaderType::Graphics: return Build(std::move(asset), { VS(), PS() });
    case ShaderType::Compute: return Build(std::move(asset), { CS() });
    }

    return {};
}


