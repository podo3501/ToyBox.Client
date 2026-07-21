#include "pch.h"
#include "TextureDescFactory.h"

TextureDesc TextureDescFactory::CreateDefault(Core::ResourceID resID)
{
    TextureDesc desc;
    desc.resID = std::move(resID);
    desc.type = TextureType::Color;
    desc.generateMips = false;
    return desc;
}

TextureDesc TextureDescFactory::CreatePath(std::string_view path)
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
