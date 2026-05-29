#include "pch.h"
#include "MaterialDescFactory.h"

MeshMaterialDesc MeshMaterialDescFactory::CreateLit(std::vector<TextureDesc> textures)
{
    MeshMaterialDesc desc;

    desc.textures = std::move(textures);
    desc.pipelineState = PipelineLibrary::Get(
        ShaderID::Mesh,
        RasterPreset::Default,
        PrimitiveTopologyType::Triangle);
    desc.surface = { 0.5f, 0.f };

    return desc;
}

MeshMaterialDesc MeshMaterialDescFactory::CreateGrid()
{
    MeshMaterialDesc desc;

    desc.pipelineState = PipelineLibrary::Get(
        ShaderID::Grid,
        RasterPreset::Default,
        PrimitiveTopologyType::Line);
    desc.surface = { 0.5f, 0.f };

    return desc;
}

MeshMaterialDesc MeshMaterialDescFactory::CreateWireframe()
{
    MeshMaterialDesc desc;

    desc.pipelineState = PipelineLibrary::Get(
        ShaderID::Mesh,
        RasterPreset::Wireframe,
        PrimitiveTopologyType::Triangle);
    desc.surface = { 0.5f, 0.f };

    return desc;
}

/////////////////////////////////////////////////////////////////

UIMaterialDesc UIMaterialDescFactory::CreateDefault(std::vector<TextureDesc> textures)
{
    UIMaterialDesc desc;

    desc.textures = std::move(textures);
    desc.pipelineState =
        PipelineLibrary::Get(
            ShaderID::UI,
            RasterPreset::NoCull,
            PrimitiveTopologyType::Triangle);

    return desc;
}