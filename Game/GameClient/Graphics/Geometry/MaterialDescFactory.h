#pragma once
#include "GameClient/Service/Render/Desc/PhongMaterialDesc.h"
#include "GameClient/Service/Render/Desc/PbrMaterialDesc.h"
#include "GameClient/Service/Render/Desc/GridMaterialDesc.h"
#include "GameClient/Service/Render/Desc/UIMaterialDesc.h"
#include "TextureDescFactory.h"

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
