#pragma once
#include "RenderState.h"
#include "GameClient/Service/Render/Resource/IResource.h"
#include "Core/Math/Matrix.h"
#include <memory>

struct SurfaceDrawItem
{
    std::shared_ptr<IResource> mesh;
    std::shared_ptr<IResource> material;
    std::optional<ShaderID> shaderOverride;
    Core::Matrix world;
};

struct ViewDrawList
{
    std::shared_ptr<IResource> environment;

    void Clear()
    {
        environment.reset();
    }
};