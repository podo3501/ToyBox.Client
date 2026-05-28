#include "pch.h"
#include "ResourceID.h"
#include "Core/Utils/StringUtils.h"

namespace
{
    inline constexpr std::string_view kBuiltinPrefix = "builtin://";
    inline constexpr std::string_view kRuntimePrefix = "runtime://";
    inline constexpr std::string_view kFilePrefix = "file://";
}

namespace Core
{
    ResourceID ResourceID::MakeBuiltin(std::string_view name)
    {
        std::string normalized = ToLowerCopy(std::string(name));
        return ResourceID(std::string(kBuiltinPrefix) + normalized);
    }

    ResourceID ResourceID::MakeRuntime(std::string_view name)
    {
        return ResourceID(std::string(kRuntimePrefix) + std::string(name));
    }

    ResourceID ResourceID::MakeFile(const std::filesystem::path& path)
    {
        return ResourceID(std::string(kFilePrefix) + NormalizePath(path));
    }

    ResourceIDType ResourceID::GetType() const
    {
        if (m_value.empty()) return ResourceIDType::Invalid;
        if (m_value.starts_with(kBuiltinPrefix)) return ResourceIDType::Builtin;
        if (m_value.starts_with(kRuntimePrefix)) return ResourceIDType::Runtime;
        if (m_value.starts_with(kFilePrefix)) return ResourceIDType::File;

        return ResourceIDType::Invalid;
    }

    std::string_view ResourceID::GetValue() const
    {
        switch (GetType())
        {
        case ResourceIDType::Builtin: return std::string_view(m_value).substr(kBuiltinPrefix.size());
        case ResourceIDType::Runtime: return std::string_view(m_value).substr(kRuntimePrefix.size());
        case ResourceIDType::File: return std::string_view(m_value).substr(kFilePrefix.size());
        default:
            return {};
        }
    }

    bool ResourceID::IsValid() const
    {
        return GetType() != ResourceIDType::Invalid;
    }
}