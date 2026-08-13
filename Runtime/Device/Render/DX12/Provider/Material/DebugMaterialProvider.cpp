#include "pch.h"
#include "DebugMaterialProvider.h"
#include "../Texture/TextureProvider.h"
#include "GameClient/Asset/DebugMaterialAsset.h"
#include "Resource/Material/GridDebugMaterialResource.h"
#include "Core/Foundation/Cast.hpp"

DebugMaterialProvider::~DebugMaterialProvider() = default;
DebugMaterialProvider::DebugMaterialProvider() noexcept
{}

std::shared_ptr<IResource> DebugMaterialProvider::CreateResource(std::shared_ptr<AssetData> asset)
{
    if (!asset) return nullptr;

    std::shared_ptr<GridDebugMaterialResource> debugRes;

    auto debugAsset = Core::Cast<DebugMaterialAsset>(asset);
    switch (debugAsset->type)
    {
    case DebugMaterialType::Grid:
        debugRes = std::make_shared<GridDebugMaterialResource>();
    }
    Assert(debugRes);

    return debugRes;
}

void DebugMaterialProvider::ReleaseResource(std::shared_ptr<IResource> resource)
{
    //resource는 여기를 벗어나면 삭제된다.
    return;
}
