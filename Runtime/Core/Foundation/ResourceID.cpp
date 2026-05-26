#include "pch.h"
#include "ResourceID.h"
#include "Core/Utils/StringUtils.h"

inline constexpr std::string_view kBuiltinPrefix = "builtin://";
inline constexpr std::string_view kRuntimePrefix = "runtime://";
inline constexpr std::string_view kFilePrefix = "file://";

Core::ResourceID Core::MakeBuiltinResourceID(std::string_view name)
{
    std::string normalized = ToLowerCopy(std::string(name));
    return std::string(kBuiltinPrefix) + normalized;
}

Core::ResourceID Core::MakeRuntimeResourceID(std::string_view name)
{
    return std::string(kRuntimePrefix) + std::string(name);
}

Core::ResourceID Core::MakeFileResourceID(const std::filesystem::path& path)
{
    return std::string(kFilePrefix) + NormalizePath(path);
}

Core::ResourceIDType Core::GetResourceIDType(const ResourceID& id)
{
    if (id.empty()) return ResourceIDType::Invalid;
    if (id.starts_with(kBuiltinPrefix)) return ResourceIDType::Builtin;
    if (id.starts_with(kRuntimePrefix)) return ResourceIDType::Runtime;
    if (id.starts_with(kFilePrefix)) return ResourceIDType::File;

    return ResourceIDType::Invalid;
}

std::string_view Core::GetResourceName(const ResourceID& id)
{
    switch (GetResourceIDType(id))
    {
    case ResourceIDType::Builtin: return std::string_view(id).substr(kBuiltinPrefix.size());
    case ResourceIDType::Runtime: return std::string_view(id).substr(kRuntimePrefix.size());
    case ResourceIDType::File: return std::string_view(id).substr(kFilePrefix.size());
    default:
        return {};
    }
}

bool Core::IsValid(const ResourceID& id)
{
    return GetResourceIDType(id) != ResourceIDType::Invalid;
}