#pragma once

//App을 띄울때 필요한 최소한의 정보를 저장한다. 리소스나 해상도 같은.
class Environment
{
public:
	Environment() = delete;
	Environment(const wstring& resourcePath, const Vector2& resolution);

	inline const filesystem::path& GetResourcePath() const noexcept { return m_resourcePath; }
	const filesystem::path& GetResourceFontPath() const noexcept { return m_fontPath; }
	const Vector2& GetResolution() const noexcept { return m_resolution; }
	Rectangle GetRectResolution() const noexcept;
	filesystem::path GetResourceFullFilename(const filesystem::path& filename) const noexcept;
	filesystem::path GetRelativePath(const filesystem::path& fullPath) const noexcept;

private:
	filesystem::path m_fontPath;
	filesystem::path m_resourcePath;
	Vector2 m_resolution{};
};