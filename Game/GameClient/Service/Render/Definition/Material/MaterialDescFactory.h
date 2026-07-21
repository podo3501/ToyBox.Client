#pragma once
#include "PhongMaterialDesc.h"
#include "PbrMaterialDesc.h"
#include "GridMaterialDesc.h"
#include "UIMaterialDesc.h"
#include "../Texture/TextureDescFactory.h"

struct PhongTextureArgs
{
    TextureDesc albedo{};
    TextureDesc normal{};
};

struct PbrTextureArgs
{
    TextureDesc albedo{};
    TextureDesc normal{};
    TextureDesc arm{}; //R: AO, G: Roughness, B: Metallic
};

namespace SurfaceMatDescFactory
{
    namespace Phong
    {
        PhongMaterialDesc CreateLit(const PhongTextureArgs& texArgs = {});
        PhongMaterialDesc CreateWireframe();
    }

    namespace PBR
    {
        PbrMaterialDesc CreateLit(const PbrTextureArgs& texArgs = {});
    }
}

namespace DebugSurfaceMatDescFactory
{
    namespace Grid
    {
        GridMaterialDesc CreateGrid();
    }
}

struct UITextureArgs
{
    TextureDesc normal{};
};

namespace UIMatDescFactory
{
    UIMaterialDesc CreateDefault(const UITextureArgs& texArgs = {});
}
