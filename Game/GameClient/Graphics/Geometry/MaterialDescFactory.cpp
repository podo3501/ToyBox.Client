#include "pch.h"
#include "MaterialDescFactory.h"

namespace SurfaceMatDescFactory
{
    namespace Phong
    {
        PhongMaterialDesc CreateLit(const PhongTextureArgs& texArgs)
        {
            PhongMaterialDesc desc{};

            desc.textures.resize(static_cast<size_t>(PhongTextureSlot::Count));
            desc.textures[static_cast<size_t>(PbrTextureSlot::Albedo)] = texArgs.albedo;
            desc.textures[static_cast<size_t>(PbrTextureSlot::Normal)] = texArgs.normal;

            return desc;
        }

        PhongMaterialDesc CreateWireframe()
        {
            PhongMaterialDesc desc{};

            desc.pipelineState = PipelineLibrary::Get(
                ShadingModel::Phong,
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

            desc.textures.resize(static_cast<size_t>(PbrTextureSlot::Count));
            desc.textures[static_cast<size_t>(PbrTextureSlot::Albedo)] = texArgs.albedo;
            desc.textures[static_cast<size_t>(PbrTextureSlot::Normal)] = texArgs.normal;
            desc.textures[static_cast<size_t>(PbrTextureSlot::ARM)] = texArgs.arm;

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

    desc.textures.resize(static_cast<size_t>(UITextureSlot::Count));
    desc.textures[static_cast<size_t>(UITextureSlot::Normal)] = texArgs.normal;

    return desc;
}