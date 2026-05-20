#include "pch.h"
#include "MaterialDescFactory.h"

std::unique_ptr<MeshMaterialDesc> MeshMaterialDescFactory::CreateLit()
{
    auto desc = std::make_unique<MeshMaterialDesc>();
    desc->pipelineState = PipelineLibrary::Get(
        ShaderID::Mesh,
        RasterPreset::Default,
        PrimitiveTopologyType::Triangle);
    desc->albedoDesc = { true, true };
    desc->surface = { 0.5f, 0.f };

    return desc;
}

std::unique_ptr<MeshMaterialDesc> MeshMaterialDescFactory::CreateGrid()
{
    auto desc = std::make_unique<MeshMaterialDesc>();
    desc->pipelineState = PipelineLibrary::Get(
        ShaderID::Mesh,
        RasterPreset::Default,
        PrimitiveTopologyType::Line);
    desc->albedoDesc = { true, false };
    desc->surface = { 0.5f, 0.f };

    return desc;
}

std::unique_ptr<MeshMaterialDesc> MeshMaterialDescFactory::CreateWireframe()
{
    auto desc = std::make_unique<MeshMaterialDesc>();
    desc->pipelineState = PipelineLibrary::Get(
        ShaderID::Mesh,
        RasterPreset::Wireframe,
        PrimitiveTopologyType::Triangle);
    desc->albedoDesc = { true, false };
    desc->surface = { 0.5f, 0.f };

    return desc;
}

/////////////////////////////////////////////////////////////////

std::unique_ptr<UIMaterialDesc> UIMaterialDescFactory::CreateDefault()
{
    auto desc = std::make_unique<UIMaterialDesc>();
    desc->pipelineState =
        PipelineLibrary::Get(
            ShaderID::UI,
            RasterPreset::NoCull,
            PrimitiveTopologyType::Triangle);
    desc->texDesc = { true, false };

    return desc;
}