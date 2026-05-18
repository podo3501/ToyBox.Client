#include "pch.h"
#include "MaterialFactory.h"

MaterialDesc MaterialFactory::CreateLit()
{
    MaterialDesc desc;
    desc.pipelineState =
        PipelineLibrary::Get(
            RasterPreset::Default,
            PrimitiveTopologyType::Triangle);

    return desc;
}

MaterialDesc MaterialFactory::CreateGrid()
{
    MaterialDesc desc;
    desc.pipelineState =
        PipelineLibrary::Get(
            RasterPreset::Default,
            PrimitiveTopologyType::Line);

    return desc;
}

MaterialDesc MaterialFactory::CreateWireframe()
{
    MaterialDesc desc;
    desc.pipelineState =
        PipelineLibrary::Get(
            RasterPreset::Wireframe,
            PrimitiveTopologyType::Triangle);

    return desc;
}
