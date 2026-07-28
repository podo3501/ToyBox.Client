#pragma once
#include "PhongMaterialDesc.h"
#include "PbrMaterialDesc.h"
#include "GridMaterialDesc.h"
#include "UIMaterialDesc.h"

struct PhongTextureArgs
{
    Core::ResourceID albedo{};
    Core::ResourceID normal{};
};

struct PbrTextureArgs
{
    Core::ResourceID albedo;
    Core::ResourceID normal{};
    Core::ResourceID arm{}; //R: AO, G: Roughness, B: Metallic
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
    Core::ResourceID normal{};
};

namespace UIMatDescFactory
{
    UIMaterialDesc CreateDefault(const UITextureArgs& texArgs = {});
}
