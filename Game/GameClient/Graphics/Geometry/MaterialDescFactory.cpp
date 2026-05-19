#include "pch.h"
#include "MaterialDescFactory.h"

MaterialDesc MaterialDescFactory::CreateLit()
{
    MaterialDesc desc;
    desc.pipelineState =
        PipelineLibrary::Get(
            ShaderID::Mesh,
            RasterPreset::Default,
            PrimitiveTopologyType::Triangle);

    return desc;
}

MaterialDesc MaterialDescFactory::CreateGrid()
{
    MaterialDesc desc;
    desc.pipelineState =
        PipelineLibrary::Get(
            ShaderID::Mesh,
            RasterPreset::Default,
            PrimitiveTopologyType::Line);

    return desc;
}

MaterialDesc MaterialDescFactory::CreateWireframe()
{
    MaterialDesc desc;
    desc.pipelineState =
        PipelineLibrary::Get(
            ShaderID::Mesh,
            RasterPreset::Wireframe,
            PrimitiveTopologyType::Triangle);

    return desc;
}
