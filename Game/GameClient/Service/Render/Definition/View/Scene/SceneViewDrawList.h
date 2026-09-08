#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"
#include "GameClient/Service/Render/Definition/Shader/ShaderTypes.h"
#include "Core/Math/Matrix.h"
#include <memory>
#include <vector>
#include <optional>

struct DrawSurfaceItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> material;
    std::optional<ShaderID> shaderOverride;
    Core::Matrix world;
};

struct DrawDebugSurfaceItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> material;
    Core::Matrix world;
};

struct SceneViewDrawList
{
    std::shared_ptr<IResource> environment;
    std::vector<DrawSurfaceItem> surfaces;
    std::vector<DrawDebugSurfaceItem> debugSurfaces;

    bool IsEmpty() const
    {
        return !environment && surfaces.empty() && debugSurfaces.empty();
    }

    void Clear()
    {
        environment.reset();
        surfaces.clear();
        debugSurfaces.clear();
    }
};