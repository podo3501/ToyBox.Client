#include "pch.h"
#include "MeshDescFactory.h"
#include "Core/Foundation/ResourceID.h"

MeshDesc MeshDescFactory::CreateDefault(Core::ResourceID resID)
{
    MeshDesc desc;
    desc.resID = std::move(resID);
    return desc;
}

MeshDesc MeshDescFactory::CreatePath(std::string_view path)
{
    return CreateDefault(Core::ResourceID::MakePath(path));
}

MeshDesc MeshDescFactory::CreateRuntime(std::string_view name)
{
    return CreateDefault(Core::ResourceID::MakeRuntime(name));
}

MeshDesc MeshDescFactory::CreateBuiltin(std::string_view name)
{
    return CreateDefault(Core::ResourceID::MakeBuiltin(name));
}