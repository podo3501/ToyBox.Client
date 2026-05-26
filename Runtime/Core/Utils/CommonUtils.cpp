#include "pch.h"
#include "CommonUtils.h"
#include "StringUtils.h"

std::string NormalizePath(const std::filesystem::path& path)
{
    std::string s = path.lexically_normal().generic_string();
    return ToLowerCopy(s);
}