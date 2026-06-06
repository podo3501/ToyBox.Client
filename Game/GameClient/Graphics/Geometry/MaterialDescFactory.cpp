#include "pch.h"
#include "MaterialDescFactory.h"

PbrMaterialDesc MeshMaterialDescFactory::CreateLit(const PbrTextureArgs& texArgs)
{
    PbrMaterialDesc desc;

    desc.textures.resize(static_cast<size_t>(PbrTextureSlot::Count));
    desc.textures[static_cast<size_t>(PbrTextureSlot::Albedo)] = texArgs.albedo;
    desc.textures[static_cast<size_t>(PbrTextureSlot::Normal)] = texArgs.normal;
    desc.textures[static_cast<size_t>(PbrTextureSlot::ARM)] = texArgs.arm;

    return desc;
}

GridMaterialDesc MeshMaterialDescFactory::CreateGrid()
{
    GridMaterialDesc desc;
    return desc;
}

PbrMaterialDesc MeshMaterialDescFactory::CreateWireframe()
{
    PbrMaterialDesc desc;

    desc.pipelineState = PipelineLibrary::Get(
        ShadingModel::PBR,
        RasterPreset::Wireframe,
        PrimitiveTopologyType::Triangle);

    return desc;
}

/////////////////////////////////////////////////////////////////

UIMaterialDesc UIMaterialDescFactory::CreateDefault(const UITextureArgs& texArgs)
{
    UIMaterialDesc desc;

    desc.textures.resize(static_cast<size_t>(UITextureSlot::Count));
    desc.textures[static_cast<size_t>(UITextureSlot::Normal)] = texArgs.normal;

    return desc;
}