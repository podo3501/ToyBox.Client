#include "pch.h"
#include "MaterialDescFactory.h"

namespace SurfaceMatDescFactory
{
    namespace Phong
    {
        PhongMaterialDe CreateLit(const PhongTextureArgs& texArgs)
        {
            PhongMaterialDe desc{};
            desc.textures.emplace_back(Resolve(PhongTextureSlot::Albedo), texArgs.albedo);
            desc.textures.emplace_back(Resolve(PhongTextureSlot::Normal), texArgs.normal);

            return desc;
        }

        PhongMaterialDe CreateWireframe()
        {
            PhongMaterialDe desc{};

            desc.pipelineState = PipelineLibrary::Get(
                RegistryShader::Phong,
                RasterPreset::Wireframe,
                PrimitiveTopologyType::Triangle);

            return desc;
        }
    }

    namespace PBR
    {
        PbrMaterialDe CreateLit(const PbrTextureArgs& texArgs)
        {
            PbrMaterialDe desc{};
            desc.textures.emplace_back(Resolve(PbrTextureSlot::Albedo), texArgs.albedo);
            desc.textures.emplace_back(Resolve(PbrTextureSlot::Normal), texArgs.normal);
            desc.textures.emplace_back(Resolve(PbrTextureSlot::ARM), texArgs.arm);

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