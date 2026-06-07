#include "pch.h"
#include "ShaderStageBuilder.h"

ShaderStageDesc ShaderStageBuilder::VS(std::string entry)
{
    return
    {
        ShaderStage::Vertex,
        std::move(entry),
        "vs_6_6"
    };
}

ShaderStageDesc ShaderStageBuilder::PS(std::string entry)
{
    return
    {
        ShaderStage::Pixel,
        std::move(entry),
        "ps_6_6"
    };
}

ShaderStageDesc ShaderStageBuilder::CS(std::string entry)
{
    return
    {
        ShaderStage::Compute,
        std::move(entry),
        "cs_6_6"
    };
}

ShaderRegisterDesc ShaderBuilder::Build(
    ShadingModel model,
    std::shared_ptr<ShaderAsset> asset)
{
    ShaderRegisterDesc regiDesc;
    regiDesc.model = model;
    regiDesc.asset = std::move(asset);

    switch (model)
    {
    case ShadingModel::Phong:
    case ShadingModel::PBR:
    case ShadingModel::UI:
    case ShadingModel::Grid:
    {
        regiDesc.stages = { ShaderStageBuilder::VS(), ShaderStageBuilder::PS() };
        break;
    }

    case ShadingModel::MipGenerator:
    {
        regiDesc.stages = { ShaderStageBuilder::CS() };
        break;
    }
    }

    return regiDesc;
}