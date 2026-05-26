#include "pch.h"
#include "MaterialDescFactory.h"

std::unique_ptr<MeshMaterialDesc> MeshMaterialDescFactory::CreateLit(std::vector<TextureBinding> textures)
{
    auto desc = std::make_unique<MeshMaterialDesc>();

    desc->textures = std::move(textures);
    desc->pipelineState = PipelineLibrary::Get(
        ShaderID::Mesh,
        RasterPreset::Default,
        PrimitiveTopologyType::Triangle);
    desc->surface = { 0.5f, 0.f };

    return desc;
}

std::unique_ptr<MeshMaterialDesc> MeshMaterialDescFactory::CreateGrid(std::vector<TextureBinding> textures)
{
    auto desc = std::make_unique<MeshMaterialDesc>();

    desc->textures = std::move(textures);
    desc->pipelineState = PipelineLibrary::Get(
        ShaderID::Grid,
        RasterPreset::Default,
        PrimitiveTopologyType::Line);
    desc->surface = { 0.5f, 0.f };

    return desc;
}

std::unique_ptr<MeshMaterialDesc> MeshMaterialDescFactory::CreateWireframe(std::vector<TextureBinding> textures)
{
    auto desc = std::make_unique<MeshMaterialDesc>();

    desc->textures = std::move(textures);
    desc->pipelineState = PipelineLibrary::Get(
        ShaderID::Mesh,
        RasterPreset::Wireframe,
        PrimitiveTopologyType::Triangle);
    desc->surface = { 0.5f, 0.f };

    return desc;
}

/////////////////////////////////////////////////////////////////

std::unique_ptr<UIMaterialDesc> UIMaterialDescFactory::CreateDefault(std::vector<TextureBinding> textures)
{
    auto desc = std::make_unique<UIMaterialDesc>();

    desc->textures = std::move(textures);
    desc->pipelineState =
        PipelineLibrary::Get(
            ShaderID::UI,
            RasterPreset::NoCull,
            PrimitiveTopologyType::Triangle);

    return desc;
}