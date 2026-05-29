#pragma once
#include "GameClient/Service/Render/Desc/MeshDesc.h"

class MeshDescFactory
{
public:
    static MeshDesc CreateDefault(Core::ResourceID resID);

    //helper
    static MeshDesc CreatePath(const std::filesystem::path& path);
    static MeshDesc CreateRuntime(std::string_view name);
    static MeshDesc CreateBuiltin(std::string_view name);
};