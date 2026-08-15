#pragma once
#include "PhongMaterialDe.h"
#include "PbrMaterialDe.h"
#include "GridMaterialDesc.h"

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
        PhongMaterialDe CreateLit(const PhongTextureArgs& texArgs = {});
        PhongMaterialDe CreateWireframe();
    }

    namespace PBR
    {
        PbrMaterialDe CreateLit(const PbrTextureArgs& texArgs = {});
    }
}

namespace DebugSurfaceMatDescFactory
{
    namespace Grid
    {
        GridMaterialDesc CreateGrid();
    }
}