#include "pch.h"
#include "MaterialDescFactory.h"

namespace SurfaceMatDescFactory
{
    namespace Phong
    {
        PhongMaterialDesc CreateLit(const PhongTextureArgs& texArgs)
        {
            PhongMaterialDesc desc{};
            desc.textures[Resolve(PhongTextureSlot::Albedo)] = texArgs.albedo;
            desc.textures[Resolve(PhongTextureSlot::Normal)] = texArgs.normal;

            return desc;
        }

        PhongMaterialDesc CreateWireframe()
        {
            PhongMaterialDesc desc{};

            desc.pipelineState = PipelineLibrary::Get(
                RegistryShader::Phong,
                RasterPreset::Wireframe,
                PrimitiveTopologyType::Triangle);

            return desc;
        }
    }

    namespace PBR
    {
        PbrMaterialDesc CreateLit(const PbrTextureArgs& texArgs)
        {
            PbrMaterialDesc desc{};
            desc.textures[Resolve(PbrTextureSlot::Albedo)] = texArgs.albedo;
            desc.textures[Resolve(PbrTextureSlot::Normal)] = texArgs.normal;
            desc.textures[Resolve(PbrTextureSlot::ARM)] = texArgs.arm;

            return desc;
        }
    }
}

namespace DebugSurfaceMatDescFactory
{
    namespace Grid
    {
        GridMaterialDesc CreateGrid()
        {
            GridMaterialDesc desc{};
            return desc;
        }
    }
}


/////////////////////////////////////////////////////////////////

UIMaterialDesc UIMatDescFactory::CreateDefault(const UITextureArgs& texArgs)
{
    UIMaterialDesc desc{};
    desc.textures[Resolve(UITextureSlot::Normal)] = texArgs.normal;

    return desc;
}