#include "pch.h"
#include "TextureDescFactory.h"
#include "Core/Foundation/ResourceID.h"

TextureDesc TextureDescFactory::CreateDefault(Core::ResourceID resID)
{
    TextureDesc desc;
    desc.resID = std::move(resID);
    desc.srgb = true;
    desc.generateMips = false;
    return desc;
}

TextureDesc TextureDescFactory::CreatePath(const std::filesystem::path& path)
{
    return CreateDefault(Core::ResourceID::MakePath(path));
}

TextureDesc TextureDescFactory::CreateRuntime(std::string_view name)
{
    return CreateDefault(Core::ResourceID::MakeRuntime(name));
}

TextureDesc TextureDescFactory::CreateBuiltin(std::string_view name)
{
    return CreateDefault(Core::ResourceID::MakeBuiltin(name));
}
