#pragma once

namespace Core
{
    using ResourceID = std::string;
    enum class ResourceIDType
    {
        Invalid,
        Builtin,
        Runtime,
        File,
    };

    ResourceID MakeFileResourceID(const std::filesystem::path& path);
    ResourceID MakeRuntimeResourceID(std::string_view name);
    ResourceID MakeBuiltinResourceID(std::string_view name);

    ResourceIDType GetResourceIDType(const ResourceID& id);
    std::string_view GetResourceName(const ResourceID& id);

    bool IsValid(const ResourceID& id);
}
