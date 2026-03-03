#include "pch.h"
#include "Environment.h"
#include "Platform/Utils/StringExt.h"

Environment::Environment(const wstring& resourcePathW, const Vector2& resolution) :
	m_fontPath{ "UI/Font/" },
	m_resourcePath{ WStringToString(resourcePathW) },
	m_resolution{ resolution }
{}

Rectangle Environment::GetRectResolution() const noexcept
{
	Rectangle rect{ 0, 0, static_cast<long>(m_resolution.x), static_cast<long>(m_resolution.y) };
	return rect;
}

filesystem::path Environment::GetResourceFullFilename(const filesystem::path& filename) const noexcept
{
	if (filename.is_absolute())
		return filename;

	return m_resourcePath / filename;
}

filesystem::path Environment::GetRelativePath(const filesystem::path& fullPath) const noexcept
{
	if (fullPath.empty()) return {};
	return filesystem::relative(fullPath, m_resourcePath);
}
