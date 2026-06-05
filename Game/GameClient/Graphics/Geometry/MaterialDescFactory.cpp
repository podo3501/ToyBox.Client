#include "pch.h"
#include "MaterialDescFactory.h"

MeshMaterialDesc MeshMaterialDescFactory::CreateLit(const MeshTextureArgs& texArgs)
{
    MeshMaterialDesc desc;

    desc.textures.resize(3);
    desc.textures[0] = texArgs.albedo;
    desc.textures[1] = texArgs.normal;
    desc.textures[2] = texArgs.arm;

    desc.pipelineState = PipelineLibrary::Get(
        ShaderID::Mesh,
        RasterPreset::Default,
        PrimitiveTopologyType::Triangle);

    return desc;
}

MeshMaterialDesc MeshMaterialDescFactory::CreateGrid()
{
    MeshMaterialDesc desc;

    desc.pipelineState = PipelineLibrary::Get(
        ShaderID::Grid,
        RasterPreset::Default,
        PrimitiveTopologyType::Line);

    return desc;
}

MeshMaterialDesc MeshMaterialDescFactory::CreateWireframe()
{
    MeshMaterialDesc desc;

    desc.pipelineState = PipelineLibrary::Get(
        ShaderID::Mesh,
        RasterPreset::Wireframe,
        PrimitiveTopologyType::Triangle);

    return desc;
}

/////////////////////////////////////////////////////////////////

UIMaterialDesc UIMaterialDescFactory::CreateDefault(const UITextureArgs& texArgs)
{
    UIMaterialDesc desc;

    desc.textures.resize(1);
    desc.textures[0] = texArgs.normal;

    desc.pipelineState =
        PipelineLibrary::Get(
            ShaderID::UI,
            RasterPreset::NoCull,
            PrimitiveTopologyType::Triangle);

    return desc;
}