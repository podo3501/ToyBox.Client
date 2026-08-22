#pragma once
#include "GameClient/Service/Render/Resource/IResource.h"
#include "Core/Math/Matrix.h"
#include <memory>

struct DrawShadowCasterItem
{
    std::shared_ptr<IResource> mesh;
    Core::Matrix world;
};