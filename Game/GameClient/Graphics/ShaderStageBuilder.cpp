#include "pch.h"
#include "ShaderStageBuilder.h"

ShaderStageDesc ShaderStageBuilder::VS(std::string entry)
{
    return
    {
        ShaderStage::Vertex,
        std::move(entry),
        "vs_5_0"
    };
}

ShaderStageDesc ShaderStageBuilder::PS(std::string entry)
{
    return
    {
        ShaderStage::Pixel,
        std::move(entry),
        "ps_5_0"
    };
}

ShaderStageDesc ShaderStageBuilder::CS(std::string entry)
{
    return
    {
        ShaderStage::Compute,
        std::move(entry),
        "cs_5_0"
    };
}

ShaderRegisterDesc ShaderBuilder::Build(
    ShaderID shaderID,
    std::shared_ptr<ShaderAsset> asset)
{
    ShaderRegisterDesc regiDesc;
    regiDesc.shaderID = shaderID;
    regiDesc.asset = std::move(asset);

    switch (shaderID)
    {
    case ShaderID::Mesh:
    case ShaderID::UI:
    case ShaderID::Grid:
    {
        regiDesc.stages = { ShaderStageBuilder::VS(), ShaderStageBuilder::PS() };
        break;
    }

    case ShaderID::MipGenerator:
    {
        regiDesc.stages = { ShaderStageBuilder::CS() };
        break;
    }
    }

    return regiDesc;
}