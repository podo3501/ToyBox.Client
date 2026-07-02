#pragma once
#include "MeshDesc.h"

class MeshDescFactory
{
public:
    static MeshDesc CreateDefault(Core::ResourceID resID);

    //helper
    static MeshDesc CreatePath(std::string_view path);
    static MeshDesc CreateRuntime(std::string_view name);
    static MeshDesc CreateBuiltin(std::string_view name);
};