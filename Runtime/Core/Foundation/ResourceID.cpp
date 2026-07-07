#include "pch.h"
#include "ResourceID.h"
#include "Core/Utils/StringUtils.h"
#include "Assert.h"

namespace
{
    inline constexpr std::string_view kBuiltinPrefix = "builtin://";
    inline constexpr std::string_view kRuntimePrefix = "runtime://";
    inline constexpr std::string_view kPathPrefix = "path://";
}

namespace Core
{
    ResourceID ResourceID::MakeBuiltin(std::string_view name)
    {
        return ResourceID(std::string(kBuiltinPrefix) + std::string(name));
    }

    ResourceID ResourceID::MakeRuntime(std::string_view name)
    {
        return ResourceID(std::string(kRuntimePrefix) + std::string(name));
    }

    ResourceID ResourceID::MakePath(std::string_view path)
    {
        std::filesystem::path fsPath(path);

        Assert(fsPath.has_extension());
        return ResourceID(std::string(kPathPrefix) + NormalizePath(fsPath));
    }

    ResourceIDType ResourceID::GetType() const
    {
        if (m_value.empty()) return ResourceIDType::Invalid;
        if (m_value.starts_with(kBuiltinPrefix)) return ResourceIDType::Builtin;
        if (m_value.starts_with(kRuntimePrefix)) return ResourceIDType::Runtime;
        if (m_value.starts_with(kPathPrefix)) return ResourceIDType::Path;

        return ResourceIDType::Invalid;
    }

    std::string_view ResourceID::GetValue() const
    {
        switch (GetType())
        {
        case ResourceIDType::Builtin: return std::string_view(m_value).substr(kBuiltinPrefix.size());
        case ResourceIDType::Runtime: return std::string_view(m_value).substr(kRuntimePrefix.size());
        case ResourceIDType::Path: return std::string_view(m_value).substr(kPathPrefix.size());
        default:
            return {};
        }
    }

    bool ResourceID::IsValid() const
    {
        return GetType() != ResourceIDType::Invalid;
    }
}