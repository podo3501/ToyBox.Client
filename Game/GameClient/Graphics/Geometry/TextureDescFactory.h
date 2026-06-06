#pragma once
#include "GameClient/Service/Render/Desc/TextureDesc.h"
#include "Core/Foundation/ResourceID.h"

class TextureDescFactory
{
public:
    static TextureDesc CreateDefault(Core::ResourceID resID);

    //helper
    static TextureDesc CreatePath(const std::filesystem::path& path);
    static TextureDesc CreateRuntime(std::string_view name);
    static TextureDesc CreateBuiltin(std::string_view name);
};