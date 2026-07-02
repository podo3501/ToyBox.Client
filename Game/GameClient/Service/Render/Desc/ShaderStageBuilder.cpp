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

static ShaderDesc Build(
    std::shared_ptr<ShaderAsset> asset,
    std::initializer_list<ShaderStageDesc> stages)
{
    ShaderDesc desc;
    desc.asset = std::move(asset);
    desc.stages.assign(stages.begin(), stages.end());
    return desc;
}

ShaderDesc ShaderBuilder::BuildGraphics(std::shared_ptr<ShaderAsset> asset)
{
    return Build(std::move(asset), {
        ShaderStageBuilder::VS(),
        ShaderStageBuilder::PS()
        });
}

BuiltinShaderDesc ShaderBuilder::BuildGraphics(
    ShaderKey key,
    std::shared_ptr<ShaderAsset> asset)
{
    return { key, BuildGraphics(std::move(asset)) };
}

ShaderDesc ShaderBuilder::BuildCompute(std::shared_ptr<ShaderAsset> asset)
{
    return Build(std::move(asset), {
        ShaderStageBuilder::CS()
        });
}

BuiltinShaderDesc ShaderBuilder::BuildCompute(
    ShaderKey key,
    std::shared_ptr<ShaderAsset> asset)
{
    return { key, BuildCompute(std::move(asset)) };
}